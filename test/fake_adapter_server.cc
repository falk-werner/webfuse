#include "fake_adapter_server.hpp"
#include "timeout_watcher.hpp"

#include "webfuse/core/util.h"
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

static int wf_test_fake_adapter_server_callback(
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

class FakeAdapterServer::Private: public IServer
{
public:
    Private(int port)
    : client_wsi(nullptr)
    , message_received(false)
    {
        memset(ws_protocols, 0, sizeof(struct lws_protocols) * 2);
        ws_protocols[0].name = "fs";
        ws_protocols[0].callback = &wf_test_fake_adapter_server_callback;
        ws_protocols[0].per_session_data_size = 0;
        ws_protocols[0].user = reinterpret_cast<void*>(this);

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
            last_message.assign(length, *data);
            message_received = true;
        }
    }

    void onWritable(struct lws * wsi) override
    {
        if (!queue.empty())
        {
            std::string const & message = queue.front();

            unsigned char * data = new unsigned char[LWS_PRE + message.size()];
            memcpy(&data[LWS_PRE], message.c_str(), message.size());
            lws_write(wsi, &data[LWS_PRE], message.size(), LWS_WRITE_TEXT);
            delete[] data;

            queue.pop();
            if (!queue.empty())
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
            queue.push(message);
            lws_callback_on_writable(client_wsi);
        }
    }

    struct lws * client_wsi;
    bool message_received;

    struct lws_protocols ws_protocols[2];
	struct lws_context_creation_info info;
    struct lws_context * context;
    std::vector<char> last_message;
    std::queue<std::string> queue;

};

FakeAdapterServer::FakeAdapterServer(int port)
: d(new Private(port))
{

}

FakeAdapterServer::~FakeAdapterServer()
{
    delete d;
}

void FakeAdapterServer::waitForConnection()
{
    d->waitForConnection();
}

}