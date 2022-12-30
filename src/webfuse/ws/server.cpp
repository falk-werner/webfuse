#include "webfuse/ws/server.hpp"

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
    std::string current_message;

    std::mutex mut;
    uint32_t id = 0;
    std::queue<webfuse::messagewriter> requests;
    std::unordered_map<uint32_t, std::promise<webfuse::messagereader>> pending_responses;
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
            if (wsi == data->connection)
            {
                data->connection = nullptr;
            }
            break;
        case LWS_CALLBACK_RECEIVE:
            {
                auto * fragment = reinterpret_cast<char*>(in);
                data->current_message.append(fragment, len);
                if (lws_is_final_fragment(wsi))
                {
                    try
                    {
                        webfuse::messagereader reader(data->current_message);
                        uint32_t id = reader.read_u32();
                        uint8_t message_type = reader.read_u8();

                        std::lock_guard lock(data->mut);
                        auto it = data->pending_responses.find(id);
                        if (it != data->pending_responses.end())
                        {
                            it->second.set_value(std::move(reader));
                            data->pending_responses.erase(it);
                        }
                        else
                        {
                            // ToDo: log request not found
                            std::cout << "warning: request not found: id=" << id << std::endl;
                            for(auto const & entry: data->pending_responses)
                            {
                                std::cout << "\t" << entry.first << std::endl;
                            }
                        }
                    }
                    catch(...)
                    {
                        // ToDo: log invalid message
                        std::cout << "warning: invalid message" << std::endl;
                    }
                    

                }
            }
            break;
        case LWS_CALLBACK_SERVER_WRITEABLE:
            {
                webfuse::messagewriter writer(webfuse::request_type::unknown);
                bool has_msg = false;
                bool has_more = false;

                {
                    std::lock_guard lock(data->mut);
                    has_msg = !(data->requests.empty());
                    if (has_msg)
                    {
                        has_msg = true;
                        writer = std::move(data->requests.front());
                        data->requests.pop();

                        has_more = !(data->requests.empty());
                    }
                }

                if (has_msg)
                {
                    size_t size;
                    unsigned char * raw_data = writer.get_data(size);
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
                            data.requests = std::move(std::queue<webfuse::messagewriter>());
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

    uint32_t next_id()
    {
        data.id++;
        if (0 == data.id)
        {
            data.id = 1;
        }
        return data.id;
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

messagereader ws_server::perform(messagewriter writer)
{
    std::future<messagereader> f;
    {
        std::promise<messagereader> p;
        f = p.get_future();

        std::lock_guard lock(d->data.mut);
        uint32_t id = d->next_id();
        writer.set_id(id);
        d->data.requests.emplace(std::move(writer));
        d->data.pending_responses.emplace(id, std::move(p));
    }

    lws_cancel_service(d->context);
    if(std::future_status::timeout == f.wait_for(std::chrono::seconds(10)))
    {
        throw std::runtime_error("timeout");
    }

    return std::move(f.get());
}


}