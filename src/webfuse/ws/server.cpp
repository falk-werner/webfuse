#include "webfuse/ws/server.hpp"
#include <libwebsockets.h>
#include <cstring>

#include <iostream>
#include <thread>
#include <atomic>

extern "C"
{

static int ws_server_callback(struct lws *wsi, enum lws_callback_reasons reason,
    void *user, void *in, size_t len)
{
    switch(reason)
    {
        case LWS_CALLBACK_PROTOCOL_INIT:
            std::cout << "lws: protocol init "<< std::endl;
            break;
        case LWS_CALLBACK_ESTABLISHED:
            std::cout << "lws: established "<< std::endl;
            break;
        case LWS_CALLBACK_CLOSED:
            std::cout << "lws: closed "<< std::endl;
            break;
        case LWS_CALLBACK_RECEIVE:
            std::cout << "lws: receive "<< std::endl;
            break;
        case LWS_CALLBACK_SERVER_WRITEABLE:
            std::cout << "lws: server writable "<< std::endl;
            break;
        default:
            break;
    }

    return 0;
}


}

namespace webfuse
{

class ws_server::detail
{
    detail(detail const &) = delete;
    detail& operator=(detail const &) = delete;
    detail(detail &&) = delete;
    detail& operator=(detail &&) = delete;
public:
    detail(ws_config const & config)
    : shutdown_requested(false)
    {
        memset(reinterpret_cast<void*>(protocols), 0, sizeof(protocols));
        protocols[0].name = "webfuse2";
        protocols[0].callback = &ws_server_callback;
        protocols[0].per_session_data_size = 0;
        protocols[0].user = nullptr;

        memset(reinterpret_cast<void*>(&info), 0, sizeof(info));
        info.port = config.port;
        info.protocols = protocols;
        info.vhost_name = "localhost";
        info.options = LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE | LWS_SERVER_OPTION_EXPLICIT_VHOSTS;

        context = lws_create_context(&info);

        lws_vhost * const vhost = lws_create_vhost(context, &info);
        // port = lws_get_vhost_port(vhost);


        thread = std::thread([this]() {
            while (!shutdown_requested)
            {
                lws_service(context, 0);
            }

        });
    }

    ~detail()
    {
        shutdown_requested = true;
        lws_cancel_service(context);
        thread.join();
        lws_context_destroy(context);
    }

    std::thread thread;
    std::atomic<bool> shutdown_requested;
    lws_protocols protocols[2];
    lws_context_creation_info info;
    lws_context * context;
};

ws_server::ws_server(ws_config const & config)
: d(new detail(config))
{

}

ws_server::~ws_server()
{
    delete d;
}

ws_server::ws_server(ws_server && other)
{
    this->d = other.d;
    other.d = nullptr;
}

ws_server& ws_server::operator=(ws_server && other)
{
    if (this != &other)
    {
        delete this->d;
        this->d = other.d;
        other.d = nullptr;
    }

    return *this;
}

}