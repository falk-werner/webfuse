#include "webfuse/test_util/ws_client.hpp"
#include "webfuse/test_util/invokation_handler.hpp"
#include "webfuse/test_util/json_doc.hpp"
#include "webfuse/impl/json/node.h"
#include "webfuse/status.h"

#include <libwebsockets.h>

#include <cstring>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <chrono>
#include <sstream>

#define TIMEOUT (std::chrono::seconds(10))

namespace
{

class IServer
{
public:
    virtual ~IServer() = default;
    virtual void OnConnectionEstablished(lws * wsi) = 0;
    virtual void OnConnectionError(lws * wsi) = 0;
    virtual void OnConnectionClosed(lws * wsi) = 0;
    virtual void OnMessageReceived(lws * wsi, char * data, size_t length) = 0;
    virtual int OnWritable(lws * wsi) = 0;
};

}

extern "C"
{

static int webfuse_test_WsClient_callback(
        lws * wsi,
        lws_callback_reasons reason,
        void * user,
        void * in,
        size_t len)
{
    int result = 0;
    struct lws_protocols const * protocol = lws_get_protocol(wsi);
    IServer * server = (nullptr != protocol) ? reinterpret_cast<IServer*>(protocol->user) : nullptr;

    if (nullptr != server)
    {
        switch (reason)
        {
            case LWS_CALLBACK_CLIENT_ESTABLISHED:
                server->OnConnectionEstablished(wsi);
                break;
            case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
                server->OnConnectionError(wsi);
                break;
            case LWS_CALLBACK_CLIENT_CLOSED:
                server->OnConnectionClosed(wsi);
                break;
            case LWS_CALLBACK_CLIENT_RECEIVE:
                server->OnMessageReceived(wsi, reinterpret_cast<char*>(in), len);
                break;
            case LWS_CALLBACK_SERVER_WRITEABLE:
                // fall-through
            case LWS_CALLBACK_CLIENT_WRITEABLE:
                result = server->OnWritable(wsi);
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

class WsClient::Private: public IServer
{
public:
    Private(
        InvokationHandler & handler,
        std::string const & protocol)
    : wsi_(nullptr)
    , handler_(handler)
    , protocol_(protocol)
    , conn_state(connection_state::disconnected)
    , await_response(false)
    , remote_port(0)
    , remote_use_tls(false)
    {
        IServer * self = this;

        memset(protocols, 0, sizeof(lws_protocols) * 2);
        protocols[0].name = protocol_.c_str();
        protocols[0].callback = &webfuse_test_WsClient_callback;
        protocols[0].per_session_data_size = 0;
        protocols[0].user = self;

        memset(&info, 0, sizeof(lws_context_creation_info));
        info.port = CONTEXT_PORT_NO_LISTEN;
        info.protocols = protocols;
        info.uid = -1;
        info.gid = -1;
        info.options = LWS_SERVER_OPTION_EXPLICIT_VHOSTS;

        context = lws_create_context(&info);

        lws_vhost * vhost = lws_create_vhost(context, & info);
        info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
        info.client_ssl_cert_filepath = "client-cert.pem";
        info.client_ssl_private_key_filepath = "client-key.pem";
        info.client_ssl_ca_filepath = "server-cert.pem";
        lws_init_vhost_client_ssl(&info, vhost);

        thread = std::thread(&Run, this);
    }

    ~Private()
    {
        std::unique_lock<std::mutex> lock(mutex);
        commands.push(command::shutdown);
        lock.unlock();
        lws_cancel_service(context);

        thread.join();
        lws_context_destroy(context);
    }

    bool Connect(int port, std::string const & protocol, bool use_tls)
    {
        std::unique_lock<std::mutex> lock(mutex);
        remote_port = port;
        remote_protocol = protocol;
        remote_use_tls = use_tls;
        conn_state = connection_state::connecting;
        commands.push(command::connect);

        lock.unlock();
        lws_cancel_service(context);
        lock.lock();

        convar.wait_for(lock, TIMEOUT, [&]() {
            return (conn_state != connection_state::connecting);
        });

        return (connection_state::connected == conn_state);
    }

    bool Disconnect()
    {
        std::unique_lock<std::mutex> lock(mutex);
        commands.push(command::disconnect);
        conn_state = connection_state::disconnecting;

        lock.unlock();
        lws_cancel_service(context);
        lock.lock();

       convar.wait_for(lock, TIMEOUT, [&]() {
            return (conn_state != connection_state::disconnecting);
        });

        return (connection_state::disconnected == conn_state);
    }

    std::string Invoke(std::string const & message)
    {
        std::unique_lock<std::mutex> lock(mutex);
        send_queue.push(message);
        commands.push(command::send);
        await_response = true;

        lock.unlock();
        lws_cancel_service(context);
        lock.lock();

        convar.wait_for(lock, TIMEOUT, [&]() {
            return !await_response;
        });

        return response;
    }

    void OnConnectionEstablished(lws * wsi)
    {
        std::unique_lock<std::mutex> lock(mutex);
        wsi_ = wsi;
        conn_state = connection_state::connected;
        convar.notify_all();
    }

    void OnConnectionError(lws * wsi)
    {
        std::unique_lock<std::mutex> lock(mutex);
        wsi_ = nullptr;
        conn_state = connection_state::disconnected;
        convar.notify_all();
    }

    void OnConnectionClosed(lws * wsi)
    {
        std::unique_lock<std::mutex> lock(mutex);
        if (wsi == wsi_)
        {
            wsi_ = nullptr;
            conn_state = connection_state::disconnected;
            convar.notify_all();
        }
    }

    void OnMessageReceived(lws * wsi, char * data, size_t length)
    {
        std::unique_lock<std::mutex> lock(mutex);
        if (await_response)
        {
            response = std::string(data, length);
            await_response = false;
            convar.notify_all();
        }
        else
        {
            lock.unlock();

            JsonDoc doc(std::string(data, length));
            wf_json const * request = doc.root();
            wf_json const * method = wf_impl_json_object_get(request, "method");
            wf_json const * params = wf_impl_json_object_get(request, "params");
            wf_json const * id = wf_impl_json_object_get(request, "id");

            std::ostringstream response;
            response << "{";
            try
            {
                std::string result_text = handler_.Invoke(wf_impl_json_string_get(method), params);
                if (result_text.empty()) { throw std::runtime_error("empty"); }
                response << "\"result\": " << result_text;
            }
            catch (...)
            {
                response << "\"error\": {\"code\": 1}";
            }

            response << ", \"id\": " << wf_impl_json_int_get(id) << "}";

            lock.lock();
            send_queue.push(response.str());
            commands.push(command::send);
            lock.unlock();

            lws_cancel_service(context);
        }
    }

    int OnWritable(lws * wsi)
    {
        std::unique_lock<std::mutex> lock(mutex);
        if (conn_state == connection_state::disconnecting)
        {
            return 1;
        }
        else if (!send_queue.empty())
        {
            std::string message = send_queue.front();
            send_queue.pop();
            bool has_more = !send_queue.empty();
            lock.unlock();

            unsigned char * data = new unsigned char[LWS_PRE + message.size()];
            memcpy(&data[LWS_PRE], message.c_str(), message.size());
            lws_write(wsi, &data[LWS_PRE], message.size(), LWS_WRITE_TEXT);
            delete[] data;

            if (has_more)
            {
                lws_callback_on_writable(wsi);
            }
        }

        return 0;
    }


private:
    enum class command
    {
        run,
        shutdown,
        connect,
        disconnect,
        send
    };

    enum class connection_state
    {
        disconnected,
        connected,
        connecting,
        disconnecting
    };

    static void Run(Private * self)
    {
        bool is_running = true;

        while (is_running)
        {
            switch (self->GetCommand())
            {
                case command::run:
                    lws_service(self->context, 0);
                    break;
                case command::shutdown:
                    is_running = false;
                    break;
                case command::connect:
                    {
                        struct lws_client_connect_info info;
                        memset(&info, 0 ,sizeof(struct lws_client_connect_info));
                        {
                            std::unique_lock<std::mutex> lock(self->mutex);

                            info.context = self->context;
                            info.port = self->remote_port;
                            info.address = "localhost";
                            info.path = "/";
                            info.host = info.address;
                            info.origin = info.address;
                            info.ssl_connection = (self->remote_use_tls) ? LCCSCF_USE_SSL : 0;
                            info.protocol = self->remote_protocol.c_str();
                            info.local_protocol_name = self->protocol_.c_str();
                            info.pwsi = nullptr;
                        }

                        lws_client_connect_via_info(&info);
                    }
                    break;
                case command::disconnect:
                    // fallthrough
                case command::send:
                    {
                        std::unique_lock<std::mutex> lock(self->mutex);
                        lws * wsi = self->wsi_;
                        lock.unlock();

                        if (nullptr != wsi)
                        {
                            lws_callback_on_writable(wsi);
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

    command GetCommand()
    {
        std::unique_lock<std::mutex> lock(mutex);

        command actual_command = command::run;
        if (!commands.empty())
        {
            actual_command = commands.front();
            commands.pop();
        }

        return actual_command;
    }

    lws * wsi_;
    InvokationHandler & handler_;
    std::string protocol_;
    connection_state conn_state;
    bool await_response;
    std::string response;

    lws_context_creation_info info;
    lws_protocols protocols[2];
    lws_context * context;
    std::thread thread;
    std::mutex mutex;
    std::condition_variable convar;
    std::queue<command> commands;
    int remote_port;
    std::string remote_protocol;
    bool remote_use_tls;

    std::queue<std::string> send_queue;
};

WsClient::WsClient(
    InvokationHandler& handler,
    std::string const & protocol)
: d(new Private(handler, protocol))
{

}

WsClient::~WsClient()
{
    delete d;
}

bool WsClient::Connect(int port, std::string const & protocol, bool use_tls)
{
    return d->Connect(port, protocol, use_tls);
}

bool WsClient::Disconnect()
{
    return d->Disconnect();
}

std::string WsClient::Invoke(std::string const & message)
{
    return d->Invoke(message);
}


}
