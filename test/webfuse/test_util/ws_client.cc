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
    return 0;
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
    : handler_(handler)
    , protocol_(protocol)
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

    std::future<void> Connect(int port, std::string const & protocol, bool use_tls)
    {
        {
            std::unique_lock<std::mutex> lock(mutex);
            remote_port = port;
            remote_protocol = protocol;
            remote_use_tls = use_tls;
            commands.push(command::connect);
        }

        std::promise<void> p;
        p.set_exception(std::make_exception_ptr(std::runtime_error("not implemented")));
        return p.get_future();
    }

    std::future<std::string> Invoke(std::string const & message)
    {
        std::promise<std::string> p;
        p.set_exception(std::make_exception_ptr(std::runtime_error("not implemented")));
        return p.get_future();
    }

private:
    enum class command
    {
        run,
        shutdown,
        connect
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
                    // ToDo: connect
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

    InvokationHandler & handler_;
    std::string protocol_;
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

std::future<void> WsClient::Connect(int port, std::string const & protocol, bool use_tls)
{
    return d->Connect(port, protocol, use_tls);
}

std::future<std::string> WsClient::Invoke(std::string const & message)
{
    return d->Invoke(message);
}


}