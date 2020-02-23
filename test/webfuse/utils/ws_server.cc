#include "webfuse/utils/ws_server.hpp"
#include "webfuse/utils/timeout_watcher.hpp"

#include "webfuse/core/util.h"
#include "webfuse/core/protocol_names.h"
#include <libwebsockets.h>
#include <cstring>
#include <vector>
#include <queue>
#include <string>

using webfuse_test::TimeoutWatcher;

#define DEFAULT_TIMEOUT (std::chrono::milliseconds(5 * 1000))

namespace
{

class IServer
{
public:
    virtual ~IServer() = default;
    virtual void onConnectionEstablished(struct lws * wsi) = 0;
    virtual void onConnectionClosed(struct lws * wsi) = 0;
    virtual void onMessageReceived(struct lws * wsi, char const * data, size_t length) = 0;
    virtual void onWritable(struct lws * wsi) = 0;
};

}

extern "C"
{

static int wf_test_utils_ws_server_callback(
	struct lws * wsi,
	enum lws_callback_reasons reason,
	void * WF_UNUSED_PARAM(user),
	void * in,
	size_t len)
{
    struct lws_protocols const * ws_protocol = lws_get_protocol(wsi);
    if (NULL == ws_protocol)
    {
        return 0;
    }

    auto * server = reinterpret_cast<IServer*>(ws_protocol->user);
    switch(reason)
    {
		case LWS_CALLBACK_ESTABLISHED:
            server->onConnectionEstablished(wsi);
            break;
        case LWS_CALLBACK_CLOSED:
            server->onConnectionClosed(wsi);
            break;
        case LWS_CALLBACK_RECEIVE:
            {
                auto * data = reinterpret_cast<char const *>(in);
                server->onMessageReceived(wsi, data, len);
            }
            break;
        case LWS_CALLBACK_SERVER_WRITEABLE:
            server->onWritable(wsi);
            break;
        default:
            break;
    }

    return 0;
}

}

namespace webfuse_test
{

class WebsocketServer::Private: public IServer
{
public:
    Private(int port, struct lws_protocols * additionalProtocols, size_t additionalProtocolsCount)
    : client_wsi(nullptr)
    {
        ws_protocols = new struct lws_protocols[2 + additionalProtocolsCount];
        memset(ws_protocols, 0, sizeof(struct lws_protocols) * (2 + additionalProtocolsCount));

        ws_protocols[0].name = WF_PROTOCOL_NAME_ADAPTER_SERVER;
        ws_protocols[0].callback = &wf_test_utils_ws_server_callback;
        ws_protocols[0].per_session_data_size = 0;
        ws_protocols[0].user = reinterpret_cast<void*>(this);

        if (0 < additionalProtocolsCount)
        {
            memcpy(&ws_protocols[additionalProtocolsCount], additionalProtocols, sizeof(struct lws_protocols) * additionalProtocolsCount);
        }

        memset(&info, 0, sizeof(struct lws_context_creation_info));
        info.port = port;
        info.mounts = NULL;
        info.protocols =ws_protocols;
        info.vhost_name = "localhost";
        info.ws_ping_pong_interval = 10;
        info.options = LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE;

	    context = lws_create_context(&info);

    }

    virtual ~Private()
    {
        lws_context_destroy(context);
        delete[] ws_protocols;
    }

    struct lws_context * getContext()
    {
        return context;
    }

    void waitForConnection()
    {
        TimeoutWatcher watcher(DEFAULT_TIMEOUT);

        while (nullptr == client_wsi)
        {
            watcher.check();
            lws_service(context, 100);
        }
    }

    void sendMessage(json_t * message)
    {
        char* message_text = json_dumps(message, JSON_COMPACT);
        writeQueue.push(message_text);
        json_decref(message);
        free(message_text);

        if (nullptr != client_wsi)
        {
            lws_callback_on_writable(client_wsi);

            TimeoutWatcher watcher(DEFAULT_TIMEOUT);
            while (!writeQueue.empty())
            {
                watcher.check();
                lws_service(context, 100);
            }
        }
    }

    json_t * receiveMessage()
    {
        TimeoutWatcher watcher(DEFAULT_TIMEOUT);

        while (recvQueue.empty())
        {
            watcher.check();
            lws_service(context, 100);
        }

        std::string const & message_text = recvQueue.front();
        json_t * message = json_loads(message_text.c_str(), JSON_DECODE_ANY, nullptr);
        recvQueue.pop();

        return message;
    }

    void onConnectionEstablished(struct lws * wsi) override
    {
        client_wsi = wsi;
    }

    void onConnectionClosed(struct lws * wsi) override
    {
        if (wsi == client_wsi)
        {
            client_wsi = nullptr;
        }
    }

    void onMessageReceived(struct lws * wsi, char const * data, size_t length) override
    {
        if (wsi == client_wsi)
        {
            recvQueue.push(std::string(data, length));
        }
    }

    void onWritable(struct lws * wsi) override
    {
        if (!writeQueue.empty())
        {
            std::string const & message = writeQueue.front();

            unsigned char * data = new unsigned char[LWS_PRE + message.size()];
            memcpy(&data[LWS_PRE], message.c_str(), message.size());
            lws_write(wsi, &data[LWS_PRE], message.size(), LWS_WRITE_TEXT);
            delete[] data;

            writeQueue.pop();
            if (!writeQueue.empty())
            {
                lws_callback_on_writable(wsi);
            }
        }
    }


private:
    void send(std::string const & message)
    {
        if (nullptr != client_wsi)
        {
            writeQueue.push(message);
            lws_callback_on_writable(client_wsi);
        }
    }

    struct lws * client_wsi;

    struct lws_protocols * ws_protocols;
	struct lws_context_creation_info info;
    struct lws_context * context;
    std::queue<std::string> writeQueue;
    std::queue<std::string> recvQueue;

};

WebsocketServer::WebsocketServer(int port)
: d(new Private(port, nullptr, 0))
{

}

WebsocketServer::WebsocketServer(int port, struct lws_protocols * additionalProtocols, std::size_t additionalProtocolsCount)
: d(new Private(port, additionalProtocols, additionalProtocolsCount))
{

}

WebsocketServer::~WebsocketServer()
{
    delete d;
}

struct lws_context * WebsocketServer::getContext()
{
    return d->getContext();
}

void WebsocketServer::waitForConnection()
{
    d->waitForConnection();
}

void WebsocketServer::sendMessage(json_t * message)
{
    d->sendMessage(message);
}

json_t * WebsocketServer::receiveMessage()
{
    return d->receiveMessage();
}


}