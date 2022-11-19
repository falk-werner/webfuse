#include "webfuse/ws/server.hpp"
#include "webfuse/ws/message.hpp"

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

struct user_data
{
    struct lws * connection = nullptr;

    std::mutex mut;
    std::queue<webfuse::message> requests;
    std::unordered_map<uint32_t, std::promise<std::string>> pending_responses;
};

}

extern "C"
{

static int ws_server_callback(struct lws *wsi, enum lws_callback_reasons reason,
    void *user, void *in, size_t len)
{
    auto const * protocol = lws_get_protocol(wsi);
    if (nullptr == protocol) { return 0; }
    if (&ws_server_callback != protocol->callback) { return 0; }

    auto * data = reinterpret_cast<user_data *>(protocol->user);

    int result = 0;
    switch(reason)
    {
        case LWS_CALLBACK_ESTABLISHED:
            std::cout << "lws: established "<< std::endl;
            if (nullptr == data->connection)
            {
                data->connection = wsi;
            }
            else
            {
                result = -1;
            }
            break;
        case LWS_CALLBACK_CLOSED:
            std::cout << "lws: closed "<< std::endl;
            if (wsi == data->connection)
            {
                data->connection = nullptr;
            }
            break;
        case LWS_CALLBACK_RECEIVE:
            std::cout << "lws: receive "<< std::endl;
            break;
        case LWS_CALLBACK_SERVER_WRITEABLE:
            std::cout << "lws: server writable "<< std::endl;
            {
                webfuse::message msg(webfuse::message_type::access_req);
                bool has_msg = false;
                bool has_more = false;

                {
                    std::lock_guard lock(data->mut);
                    has_msg = !(data->requests.empty());
                    if (has_msg)
                    {
                        has_msg = true;
                        msg = std::move(data->requests.front());
                        data->requests.pop();

                        has_more = !(data->requests.empty());
                    }
                }

                if (has_msg)
                {
                    size_t size;
                    unsigned char * raw_data = msg.get_data(size);
                    int const rc = lws_write(data->connection, raw_data, size, LWS_WRITE_BINARY);
                }

            }
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
    {
        memset(reinterpret_cast<void*>(protocols), 0, sizeof(protocols));
        protocols[0].name = "webfuse2";
        protocols[0].callback = &ws_server_callback;
        protocols[0].per_session_data_size = 0;
        protocols[0].user = reinterpret_cast<void*>(&data);

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
                {
                    std::lock_guard lock(data.mut);
                    if (!data.requests.empty())
                    {
                        if (nullptr != data.connection)
                        {
                            lws_callback_on_writable(data.connection);
                        }
                        else
                        {
                            data.requests = std::move(std::queue<webfuse::message>());
                            data.pending_responses.clear();
                        }
                    }
                }

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
    user_data data;
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

void ws_server::perform(message msg)
{
    std::future<std::string> f;
    {
        std::promise<std::string> p;
        f = p.get_future();

        std::lock_guard lock(d->data.mut);
        d->data.requests.emplace(std::move(msg));
        d->data.pending_responses.emplace(42, std::move(p));
    }

    lws_cancel_service(d->context);
    if(std::future_status::timeout == f.wait_for(std::chrono::seconds(1)))
    {
        throw std::runtime_error("timeout");
    }
    std::string resp = f.get();

    throw std::runtime_error("not implemented");
}


}