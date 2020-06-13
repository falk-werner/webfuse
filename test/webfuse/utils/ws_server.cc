#include "webfuse/utils/ws_server.h"
#include "webfuse/core/lws_log.h"

#include <libwebsockets.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <sstream>
#include <queue>

namespace
{

class IServer
{
public:
    virtual ~IServer() = default;
    virtual void OnConnected(lws * wsi) = 0;
    virtual void OnConnectionClosed(lws * wsi) = 0;
    virtual void OnMessageReceived(struct lws * wsi, char const * data, size_t length) = 0;
    virtual void OnWritable(struct lws * wsi) = 0;
};

}

namespace webfuse_test
{

class WsServer::Private : IServer
{
    Private(Private const &) = delete;
    Private & operator=(Private const &) = delete;
public:
    Private(std::string const & protocol, int port);
    ~Private();
    bool IsConnected();
    std::string GetUrl() const;
    void SendMessage(json_t * message);
    json_t * ReceiveMessage();
    void OnConnected(lws * wsi) override;
    void OnConnectionClosed(lws * wsi) override;
    void OnMessageReceived(struct lws * wsi, char const * data, size_t length) override;
    void OnWritable(struct lws * wsi) override;

private:
    static void run(Private * self);
    std::string protocol_;
    int port_;
    bool is_connected;
    bool is_shutdown_requested;
    lws * wsi_;
    lws_context * ws_context;
    lws_protocols ws_protocols[2];
	lws_context_creation_info info;
    std::thread context;
    std::mutex mutex;
    std::queue<std::string> writeQueue;
    std::queue<std::string> recvQueue;
};

}

extern "C"
{

static int wf_test_utils_ws_server_callback(
    struct lws * wsi,
    enum lws_callback_reasons reason,
    void * user,
    void * in,
    size_t len)
{
    int result = 0;
    struct lws_protocols const * ws_protocol = lws_get_protocol(wsi);
    auto * server = reinterpret_cast<IServer*>(nullptr != ws_protocol ? ws_protocol->user : nullptr);

    if (nullptr != server)
    {
        switch (reason)
        {
            case LWS_CALLBACK_ESTABLISHED:
                server->OnConnected(wsi);
                break;
            case LWS_CALLBACK_CLOSED:
                server->OnConnectionClosed(wsi);
                break;
            case LWS_CALLBACK_RECEIVE:
                {
                    auto * data = reinterpret_cast<char const *>(in);
                    server->OnMessageReceived(wsi, data, len);
                }
                break;
            case LWS_CALLBACK_SERVER_WRITEABLE:
                server->OnWritable(wsi);
                break;
            default:
                break;
        }
    }

    return result;
}

}


namespace webfuse_test
{

WsServer::WsServer(std::string const & protocol, int port)
: d(new Private(protocol, port))
{

}

WsServer::~WsServer()
{
    delete d;
}

bool WsServer::IsConnected()
{
    return d->IsConnected();
}

void WsServer::SendMessage(json_t * message)
{
    d->SendMessage(message);
}

json_t * WsServer::ReceiveMessage()
{
    return d->ReceiveMessage();
}

std::string WsServer::GetUrl() const
{
    return d->GetUrl();
}


WsServer::Private::Private(std::string const & protocol, int port)
: protocol_(protocol)
, port_(port)
, is_connected(false)
, is_shutdown_requested(false)
, wsi_(nullptr)
{
    wf_lwslog_disable();
    IServer * server = this;
    memset(ws_protocols, 0, sizeof(struct lws_protocols) * 2 );

    ws_protocols[0].name = protocol_.c_str();
    ws_protocols[0].callback = &wf_test_utils_ws_server_callback;
    ws_protocols[0].per_session_data_size = 0;
    ws_protocols[0].user = reinterpret_cast<void*>(server);

    memset(&info, 0, sizeof(struct lws_context_creation_info));
    info.port = port;
    info.mounts = NULL;
    info.protocols =ws_protocols;
    info.vhost_name = "localhost";
    info.ws_ping_pong_interval = 10;
    info.options = LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE;
    info.options |= LWS_SERVER_OPTION_EXPLICIT_VHOSTS;

    ws_context = lws_create_context(&info);

    struct lws_vhost * vhost = lws_create_vhost(ws_context, &info);
    port_ = lws_get_vhost_port(vhost);

    context = std::thread(&run, this);
}

WsServer::Private::~Private()
{
    {
        std::unique_lock<std::mutex> lock(mutex);
        is_shutdown_requested = true;
    }

    lws_cancel_service(ws_context);
    context.join();
    lws_context_destroy(ws_context);
}

void WsServer::Private::run(Private * self)
{
    bool is_running = true;
    while (is_running)
    {
        lws_service(self->ws_context, 0);
        {
            std::unique_lock<std::mutex> lock(self->mutex);
            is_running = !self->is_shutdown_requested;
        }
    }
}

bool WsServer::Private::IsConnected()
{
    std::unique_lock<std::mutex> lock(mutex);
    return is_connected;
}

void WsServer::Private::OnConnected(lws * wsi)
{
    std::unique_lock<std::mutex> lock(mutex);
    is_connected = true;
    wsi_ = wsi;
}

void WsServer::Private::OnConnectionClosed(lws * wsi)
{
    std::unique_lock<std::mutex> lock(mutex);
    if (wsi == wsi_)
    {
        is_connected = false;
        wsi_ = nullptr;
    }
}

void WsServer::Private::OnWritable(struct lws * wsi)
{
    bool notify = false;

    {
        std::unique_lock<std::mutex> lock(mutex);

        if (!writeQueue.empty())
        {
            std::string const & message = writeQueue.front();

            unsigned char * data = new unsigned char[LWS_PRE + message.size()];
            memcpy(&data[LWS_PRE], message.c_str(), message.size());
            lws_write(wsi, &data[LWS_PRE], message.size(), LWS_WRITE_TEXT);
            delete[] data;

            writeQueue.pop();
            notify = !writeQueue.empty();
        }
    }

    if (notify)
    {
        lws_callback_on_writable(wsi);
    }
}


void WsServer::Private::SendMessage(json_t * message)
{
    lws * wsi = nullptr;

    {
        std::unique_lock<std::mutex> lock(mutex);

        if (nullptr != wsi_)
        {
            char* message_text = json_dumps(message, JSON_COMPACT);
            writeQueue.push(message_text);
            json_decref(message);
            free(message_text);
            wsi = wsi_;
        }
    }

    if (nullptr != wsi)
    {
        lws_callback_on_writable(wsi_);
    }
}

void WsServer::Private::OnMessageReceived(struct lws * wsi, char const * data, size_t length)
{
    std::unique_lock<std::mutex> lock(mutex);
    if (wsi == wsi_)
    {
        recvQueue.push(std::string(data, length));
    }
}

json_t * WsServer::Private::ReceiveMessage()
{
    std::unique_lock<std::mutex> lock(mutex);
 
    json_t * result = nullptr;
    if (!recvQueue.empty())
    {
        std::string const & message_text = recvQueue.front();
        result = json_loads(message_text.c_str(), JSON_DECODE_ANY, nullptr);
        recvQueue.pop();
    }

    return result;
}

std::string WsServer::Private::GetUrl() const
{
    std::ostringstream stream;
    stream << "ws://localhost:" << port_ << "/";
    return stream.str();
}


}