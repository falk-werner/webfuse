#include "webfuse/ws/server.hpp"
#include "webfuse/ws/server_handler.hpp"

#include <libwebsockets.h>

#include <cinttypes>
#include <cstring>

#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <future>
#include <chrono>
#include <stdexcept>

#include <queue>
#include <string>
#include <unordered_map>


namespace
{

constexpr int64_t const timeout_secs = 10;

}

extern "C"
{

static int ws_server_callback(struct lws *wsi, enum lws_callback_reasons reason,
    void *user, void *in, size_t len)
{
    auto const * protocol = lws_get_protocol(wsi);
    if (nullptr == protocol) { return 0; }
    if (&ws_server_callback != protocol->callback) { return 0; }

    auto * handler = reinterpret_cast<webfuse::server_handler*>(protocol->user);

    int result = 0;
    switch(reason)
    {
        case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
            result = handler->filter_connection(wsi);
            break;
        case LWS_CALLBACK_ESTABLISHED:
            result = handler->on_established(wsi);
            break;
        case LWS_CALLBACK_CLOSED:
            handler->on_closed(wsi);
            break;
        case LWS_CALLBACK_RECEIVE:
            handler->on_receive(wsi, in, len);
            break;
        case LWS_CALLBACK_SERVER_WRITEABLE:
            handler->on_writable();
            break;
        default:
            break;
    }

    return result;
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
    , data(config.authenticator, config.auth_header)
    {
        lws_set_log_level(0, nullptr);

        memset(reinterpret_cast<void*>(protocols), 0, sizeof(protocols));
        protocols[0].name = "webfuse2";
        protocols[0].callback = &ws_server_callback;
        protocols[0].per_session_data_size = 0;
        protocols[0].user = reinterpret_cast<void*>(&data);

        memset(reinterpret_cast<void*>(&info), 0, sizeof(info));
        info.port = config.port;
        info.protocols = protocols;
        info.vhost_name = config.vhost_name.c_str();
        info.options = LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE | LWS_SERVER_OPTION_EXPLICIT_VHOSTS;

        if (config.use_tls)
        {
            info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
            info.ssl_cert_filepath = config.cert_path.c_str();
            info.ssl_private_key_filepath = config.key_path.c_str();
        }

        context = lws_create_context(&info);

        lws_create_vhost(context, &info);
        // lws_vhost * const vhost = lws_create_vhost(context, &info);
        // int port = lws_get_vhost_port(vhost);

        thread = std::thread([this]() {
            while (!shutdown_requested)
            {
                data.poll();
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
    server_handler data;
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

messagereader ws_server::perform(messagewriter writer)
{
    auto result = d->data.perform(std::move(writer));

    lws_cancel_service(d->context);
    if(std::future_status::timeout == result.wait_for(std::chrono::seconds(timeout_secs)))
    {
        throw std::runtime_error("timeout");
    }

    return std::move(result.get());
}


}