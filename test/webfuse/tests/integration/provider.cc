#include "webfuse/tests/integration/provider.hpp"
#include "webfuse_provider.h"
#include "webfuse/provider/impl/client.h"
#include <thread>
#include <mutex>
#include <chrono>
#include <string>
#include "webfuse/utils/static_filesystem.h"

using namespace std::chrono_literals;

namespace
{
    enum class ConnectionState
    {
        disconnected,
        connected,
        connecting
    };
}

extern "C"
{

void
webfuse_test_provider_onconnected(
    void * user_data)
{
    auto * fs = reinterpret_cast<wfp_static_filesystem*>(user_data);
    auto * connection_state = reinterpret_cast<ConnectionState*>(wfp_static_filesystem_get_user_data(fs));
    *connection_state = ConnectionState::connected;
}

void
webfuse_test_provider_ondisconnected(
    void * user_data)
{
    auto * fs = reinterpret_cast<wfp_static_filesystem*>(user_data);
    auto * connection_state = reinterpret_cast<ConnectionState*>(wfp_static_filesystem_get_user_data(fs));
    *connection_state = ConnectionState::disconnected;
}

}

namespace webfuse_test
{

class Provider::Private
{
public:
    explicit Private(char const * url)
    : is_shutdown_requested(false)
    , connection_state(ConnectionState::connecting)
    {
        config = wfp_client_config_create();
        wfp_client_config_set_certpath(config, "client-cert.pem");
        wfp_client_config_set_keypath(config, "client-key.pem");
        wfp_client_config_set_ca_filepath(config, "server-cert.pem");
        wfp_client_config_set_onconnected(config, &webfuse_test_provider_onconnected);
        wfp_client_config_set_ondisconnected(config, &webfuse_test_provider_ondisconnected);

        fs = wfp_static_filesystem_create(config);
        wfp_static_filesystem_set_user_data(fs, reinterpret_cast<void*>(&connection_state));
        wfp_static_filesystem_add_text(fs, "hello.txt", 0444, "Hello, World");

        client = wfp_client_create(config);
        wfp_client_connect(client, url);
        while (ConnectionState::connecting == connection_state)
        {
            wfp_client_service(client);
        }

        if (ConnectionState::connected == connection_state)
        {
            thread = std::thread(Run, this);
            std::this_thread::sleep_for(200ms);
        }
        else
        {
            wfp_client_dispose(client);

            wfp_static_filesystem_dispose(fs);
            wfp_client_config_dispose(config);

            throw std::runtime_error("unable to connect");
        }
    }

    ~Private()
    {
        RequestShutdown();
        thread.join();

        wfp_client_disconnect(client);
        while (ConnectionState::disconnected != connection_state)
        {
            wfp_client_service(client);
        }

        wfp_client_dispose(client);

        wfp_static_filesystem_dispose(fs);
        wfp_client_config_dispose(config);
    }

    bool IsShutdownRequested()
    {
        std::lock_guard<std::mutex> lock(shutdown_lock);
        return is_shutdown_requested;
    }
private:
    void RequestShutdown()
    {
        std::lock_guard<std::mutex> lock(shutdown_lock);
        is_shutdown_requested = true;
        wfp_client_interrupt(client);
    }

    static void Run(Provider::Private * context)
    {
        while (!context->IsShutdownRequested())
        {
            wfp_client_service(context->client);
        }
    }

    std::mutex shutdown_lock;
    std::thread thread;
    bool is_shutdown_requested;
    ConnectionState connection_state;

    wfp_client_config * config;
    wfp_static_filesystem * fs;
public:
    wfp_client * client;
};

Provider::Provider(char const * url)
: d(new Provider::Private(url))
{
}

Provider::~Provider()
{
    delete d;
}

}