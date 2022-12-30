#include "webfuse/ws/client.hpp"
#include <libwebsockets.h>
#include <cstring>
#include <iostream>

namespace
{

extern "C" int webfuse_client_callback(lws * wsi, lws_callback_reasons reason, void* user, void * in, size_t length)
{
    int result = 0;

    lws_protocols const * protocol = lws_get_protocol(wsi);

    if (nullptr != protocol)
    {
        switch(reason)
        {
            case LWS_CALLBACK_CLIENT_ESTABLISHED:
                std::cout << "established" << std::endl;
                break;
            case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
                std::cout << "connect error" << std::endl;
                break;
            case LWS_CALLBACK_CLIENT_CLOSED:
                std::cout << "closed" << std::endl;
                break;
            case LWS_CALLBACK_CLIENT_RECEIVE:
                std::cout << "receive" << std::endl;
                break;
            case LWS_CALLBACK_SERVER_WRITEABLE:
                // fall-through
            case LWS_CALLBACK_CLIENT_WRITEABLE:
                std::cout << "writable" << std::endl;
                break;
            default:
                break;
        }
    }


    return result;
}

}

namespace webfuse
{

class ws_client::detail
{
    detail(detail const &) = delete;
    detail& operator=(detail const &) = delete;
    detail(detail &&) = delete;
    detail& operator=(detail &&) = delete;
public:
    detail(ws_client_handler handler)
    : handler_(handler)
    {
        memset(reinterpret_cast<void*>(protocols), 0, sizeof(lws_protocols) * 2);
        protocols[0].callback = &webfuse_client_callback;
        protocols[0].name = "webfuse2-client";
        protocols[0].per_session_data_size = 0;
        protocols[0].user = nullptr;

        memset(reinterpret_cast<void*>(&info), 0, sizeof(lws_context_creation_info));
        info.port = CONTEXT_PORT_NO_LISTEN;
        info.protocols = protocols;
        info.uid = -1;
        info.gid = -1;

        context = lws_create_context(&info);
    }

    ~detail()
    {
        lws_context_destroy(context);
    }

    void connect(std::string const & url)
    {
        lws_client_connect_info info;
        memset(reinterpret_cast<void*>(&info), 0, sizeof(lws_client_connect_info));
        info.context = context;
        info.port = 8081;
        info.address = "localhost";
        info.host = "localhost";
        info.path = "/";
        info.origin = "localhost";
        info.ssl_connection = 0;
        info.protocol = "webfuse2";
        info.local_protocol_name = "webfuse2-client";
        info.pwsi = &wsi;

        lws_client_connect_via_info(&info);
    }

    void service()
    {
        lws_service(context, 0);
    }

private:
    ws_client_handler handler_;
    lws_context_creation_info info;
    lws_protocols protocols[2];
    lws_context * context;
    lws * wsi;
};

ws_client::ws_client(ws_client_handler handler)
: d(new detail(handler))
{

}

ws_client::~ws_client()
{
    delete d;
}

ws_client::ws_client(ws_client && other)
{
    this->d = other.d;
    other.d = nullptr;
}

ws_client& ws_client::operator=(ws_client && other)
{
    if (this != &other)
    {
        delete this->d;
        this->d = other.d;
        other.d = nullptr;
    }

    return *this;
}

void ws_client::connect(std::string url)
{
    d->connect(url);
}

void ws_client::service()
{
    d->service();
}

}