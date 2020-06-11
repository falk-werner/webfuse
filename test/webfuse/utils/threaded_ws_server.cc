#include "webfuse/utils/threaded_ws_server.h"
#include "webfuse/core/protocol_names.h"
#include "webfuse/core/lws_log.h"

#include <libwebsockets.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>


#define TIMEOUT (std::chrono::milliseconds(10 * 1000))

namespace
{

class IServer
{
public:
    virtual ~IServer() = default;
    virtual void OnConnected(lws * wsi) = 0;
    virtual void OnConnectionClosed(lws * wsi) = 0;
};

}

namespace webfuse_test
{

class ThreadedWsServer::Private : IServer
{
    Private(Private const &) = delete;
    Private & operator=(Private const &) = delete;
public:
    explicit Private(int port);
    ~Private();
    void WaitForConnection();
    void OnConnected(lws * wsi) override;
    void OnConnectionClosed(lws * wsi) override;

private:
    static void run(Private * self);
    int port_;
    bool is_connected;
    bool is_shutdown_requested;
    lws * wsi_;
    lws_context * ws_context;
    lws_protocols ws_protocols[2];
	lws_context_creation_info info;
    std::thread context;
    std::mutex mutex;
    std::condition_variable convar;
};

}

extern "C"
{

static int wf_test_utils_threaded_ws_server_callback(
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
            default:
                break;
        }
    }

    return result;
}

}


namespace webfuse_test
{

ThreadedWsServer::ThreadedWsServer(int port)
: d(new Private(port))
{

}

ThreadedWsServer::~ThreadedWsServer()
{
    delete d;
}

void ThreadedWsServer::WaitForConnection()
{
    d->WaitForConnection();
}

ThreadedWsServer::Private::Private(int port)
: port_(port)
, is_connected(false)
, is_shutdown_requested(false)
, wsi_(nullptr)
{
    wf_lwslog_disable();
    IServer * server = this;
    memset(ws_protocols, 0, sizeof(struct lws_protocols) * 2 );

    ws_protocols[0].name = WF_PROTOCOL_NAME_PROVIDER_SERVER;
    ws_protocols[0].callback = &wf_test_utils_threaded_ws_server_callback;
    ws_protocols[0].per_session_data_size = 0;
    ws_protocols[0].user = reinterpret_cast<void*>(server);

    memset(&info, 0, sizeof(struct lws_context_creation_info));
    info.port = port;
    info.mounts = NULL;
    info.protocols =ws_protocols;
    info.vhost_name = "localhost";
    info.ws_ping_pong_interval = 10;
    info.options = LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE;

    ws_context = lws_create_context(&info);

    context = std::thread(&run, this);
}

ThreadedWsServer::Private::~Private()
{
    {
        std::unique_lock<std::mutex> lock(mutex);
        is_shutdown_requested = true;
    }

    lws_cancel_service(ws_context);
    context.join();
    lws_context_destroy(ws_context);
}

void ThreadedWsServer::Private::run(Private * self)
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

void ThreadedWsServer::Private::WaitForConnection()
{
    std::unique_lock<std::mutex> lock(mutex);
    while (!is_connected)
    {
        auto status = convar.wait_for(lock, TIMEOUT);
        if (std::cv_status::timeout == status)
        {
            throw std::runtime_error("timeout");
        }
    }
}

void ThreadedWsServer::Private::OnConnected(lws * wsi)
{
    std::unique_lock<std::mutex> lock(mutex);
    is_connected = true;
    wsi_ = wsi;
    convar.notify_all();
}

void ThreadedWsServer::Private::OnConnectionClosed(lws * wsi)
{
    std::unique_lock<std::mutex> lock(mutex);
    if (wsi == wsi_)
    {
        is_connected = false;
        wsi_ = wsi;
        convar.notify_all();
    }
}


}