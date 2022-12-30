#include "webfuse/ws/client.hpp"
#include <libwebsockets.h>
#include <cstring>
#include <iostream>

#include <queue>
#include <string>

namespace
{

struct user_data
{
    webfuse::ws_client_handler handler;
    std::function<void(bool)> connection_listener;
    struct lws * connection;
    std::string current_message;
    std::queue<webfuse::messagewriter> requests;
};

extern "C" int webfuse_client_callback(lws * wsi, lws_callback_reasons reason, void* user, void * in, size_t length)
{
    int result = 0;

    lws_protocols const * protocol = lws_get_protocol(wsi);
    user_data * context = (nullptr != protocol) ? reinterpret_cast<user_data*>(protocol->user) : nullptr;

    if (nullptr != context)
    {
        switch(reason)
        {
            case LWS_CALLBACK_CLIENT_ESTABLISHED:
                std::cout << "established" << std::endl;
                context->connection_listener(true);
                break;
            case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
                // fall-through
            case LWS_CALLBACK_CLIENT_CLOSED:
                std::cout << "closed" << std::endl;
                context->connection = nullptr;
                context->requests = std::move(std::queue<webfuse::messagewriter>());
                context->current_message.clear();
                context->connection_listener(false);
                break;
            case LWS_CALLBACK_CLIENT_RECEIVE:
                {
                    std::cout << "receive" << std::endl;
                    auto * fragment = reinterpret_cast<char*>(in);
                    context->current_message.append(fragment, length);
                    if (lws_is_final_fragment(wsi))
                    {
                        try
                        {
                            webfuse::messagereader reader(context->current_message);
                            auto writer = context->handler(reader);

                            context->requests.emplace(std::move(writer));                            
                            lws_callback_on_writable(wsi);
                        }
                        catch(...)
                        {
                            // ToDo: log
                            std::cerr << "error: failed to create response" << std::endl;
                        }                        
                    }
                }
                break;
            case LWS_CALLBACK_SERVER_WRITEABLE:
                // fall-through
            case LWS_CALLBACK_CLIENT_WRITEABLE:
                {
                    std::cout << "writable" << std::endl;
                    if (!context->requests.empty())
                    {
                        auto writer = std::move(context->requests.front());
                        context->requests.pop();

                        size_t size;
                        auto * data = writer.get_data(size);
                        lws_write(wsi, data, size, LWS_WRITE_BINARY);
                    }

                    if (!context->requests.empty())
                    {
                        lws_callback_on_writable(wsi);
                    }
                }
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
    {
        memset(reinterpret_cast<void*>(protocols), 0, sizeof(lws_protocols) * 2);
        protocols[0].callback = &webfuse_client_callback;
        protocols[0].name = "webfuse2-client";
        protocols[0].per_session_data_size = 0;
        protocols[0].user = reinterpret_cast<void*>(&data);

        memset(reinterpret_cast<void*>(&info), 0, sizeof(lws_context_creation_info));
        info.port = CONTEXT_PORT_NO_LISTEN;
        info.protocols = protocols;
        info.uid = -1;
        info.gid = -1;

        data.handler = handler;
        data.connection_listener = [](bool){ };
        data.connection = nullptr;

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
        info.pwsi = &data.connection;

        lws_client_connect_via_info(&info);
    }

    void service()
    {
        lws_service(context, 0);
    }

    void interrupt()
    {
        lws_cancel_service(context);
    }

    void set_connection_listener(std::function<void(bool)> listener)
    {
        data.connection_listener = listener;
    }

private:
    lws_context_creation_info info;
    lws_protocols protocols[2];
    lws_context * context;
    user_data data;
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

void ws_client::interrupt()
{
    d->interrupt();
}

void ws_client::set_connection_listener(std::function<void(bool)> listener)
{
    d->set_connection_listener(listener);
}

}