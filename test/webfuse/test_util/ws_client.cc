#include "webfuse/test_util/ws_client.hpp"
#include "webfuse/test_util/invokation_handler.hpp"

#include <libwebsockets.h>

#include <cstring>
#include <thread>
#include <mutex>
#include <queue>

namespace
{

class IServer
{
public:
    virtual ~IServer() = default;
    virtual void OnConnectionEstablished(lws * wsi) = 0;
    virtual void OnConnectionError(lws * wsi) = 0;
    virtual void OnConnectionClosed(lws * wsi) = 0;
    virtual void OnMessageReceived(lws * wsi, void * data, size_t length) = 0;
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
                server->OnMessageReceived(wsi, in, len);
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
    , promise_connected(nullptr)
    , promise_disconnected(nullptr)
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
        Invoke(command::shutdown);
        lws_cancel_service(context);

        thread.join();
        lws_context_destroy(context);
    }

    std::future<bool> Connect(int port, std::string const & protocol, bool use_tls)
    {
        std::future<bool> result;
        {
            std::unique_lock<std::mutex> lock(mutex);
            remote_port = port;
            remote_protocol = protocol;
            remote_use_tls = use_tls;
            commands.push(command::connect);
            promise_connected = new std::promise<bool>;
            result = promise_connected->get_future();
        }
        lws_cancel_service(context);

        return result;
    }

    std::future<bool> Disconnect()
    {
        std::future<bool> result;
        {
            std::unique_lock<std::mutex> lock(mutex);
            commands.push(command::disconnect);
            promise_disconnected = new std::promise<bool>;
            result = promise_disconnected->get_future();
        }
        lws_cancel_service(context);

        return result;

    }

    std::future<std::string> Invoke(std::string const & message)
    {
        std::promise<std::string> p;
        p.set_exception(std::make_exception_ptr(std::runtime_error("not implemented")));
        return p.get_future();
    }

    void OnConnectionEstablished(lws * wsi)
    {
        std::unique_lock<std::mutex> lock(mutex);
        wsi_ = wsi;
        if (nullptr != promise_connected)
        {
            promise_connected->set_value(true);
            delete promise_connected;
            promise_connected = nullptr;
        }
    }

    void OnConnectionError(lws * wsi)
    {
        std::unique_lock<std::mutex> lock(mutex);
        if (nullptr != promise_connected)
        {
            promise_connected->set_value(false);
            delete promise_connected;
            promise_connected = nullptr;
        }
    }

    void OnConnectionClosed(lws * wsi)
    {
        std::unique_lock<std::mutex> lock(mutex);
        if (wsi == wsi_)
        {
            wsi_ = nullptr;
            if (nullptr != promise_connected)
            {
                promise_connected->set_value(false);
                delete promise_connected;
                promise_connected = nullptr;
            }
            if (nullptr != promise_disconnected)
            {
                promise_disconnected->set_value(true);
                delete promise_disconnected;
                promise_disconnected = nullptr;
            }
        }
    }

    void OnMessageReceived(lws * wsi, void * data, size_t length)
    {

    }

    int OnWritable(lws * wsi)
    {
        std::unique_lock<std::mutex> lock(mutex);
        if (nullptr != promise_disconnected)
        {
            return 1;
        }

        return 0;
    }


private:
    enum class command
    {
        run,
        shutdown,
        connect,
        disconnect
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
                    lws_callback_on_writable(self->wsi_);
                    break;
                default:
                    break;
            }
        }
    }

    command GetCommand()
    {
        std::unique_lock<std::mutex> lock(mutex);

        command command = command::run;
        if (!commands.empty())
        {
            command = commands.front();
            commands.pop();
        }

        return command;
    }

    void Invoke(command command)
    {
        std::unique_lock<std::mutex> lock(mutex);
        commands.push(command);
    }

    lws * wsi_;
    InvokationHandler & handler_;
    std::string protocol_;

    std::promise<bool> * promise_connected;
    std::promise<bool> * promise_disconnected;

    lws_context_creation_info info;
    lws_protocols protocols[2];
    lws_context * context;
    std::thread thread;
    std::mutex mutex;
    std::queue<command> commands;
    int remote_port;
    std::string remote_protocol;
    bool remote_use_tls;
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

std::future<bool> WsClient::Connect(int port, std::string const & protocol, bool use_tls)
{
    return d->Connect(port, protocol, use_tls);
}

std::future<bool> WsClient::Disconnect()
{
    return d->Disconnect();
}

std::future<std::string> WsClient::Invoke(std::string const & message)
{
    return d->Invoke(message);
}


}