#include "webfuse/test_util/server.hpp"

#include "webfuse/test_util/mountpoint_factory.hpp"
#include "webfuse/test_util/tempdir.hpp"
#include "webfuse/impl/server.h"
#include "webfuse/webfuse.h"

#include <thread>
#include <mutex>

namespace webfuse_test
{

class Server::Private
{
public:
    Private()
    : is_shutdown_requested(false)
    , tempdir("webfuse_test_server")
    {
        config = wf_server_config_create();
        wf_server_config_set_port(config, 0);
        wf_server_config_set_mountpoint_factory(config, 
            &webfuse_test_create_mountpoint,
            reinterpret_cast<void*>(const_cast<char*>(tempdir.path())));
        wf_server_config_set_keypath(config, "server-key.pem");
        wf_server_config_set_certpath(config, "server-cert.pem");

        server = wf_server_create(config);

        while (!wf_impl_server_is_operational(server))
        {
            wf_server_service(server);
        }

        thread = std::thread(Run, this);

    }

    ~Private()
    {
        RequestShutdown();
        thread.join();
        wf_server_dispose(server);
        wf_server_config_dispose(config);
    }

    char const * GetBaseDir()
    {
        return tempdir.path();
    }

    int GetPort(void) const
    {
        return wf_server_get_port(server);
    }

private:
    static void Run(Private * context)
    {
        while (!context->IsShutdownRequested())
        {
            wf_server_service(context->server);
        }
    }

    bool IsShutdownRequested()
    {
        std::lock_guard<std::mutex> lock(shutdown_lock);
        return is_shutdown_requested;
    }

    void RequestShutdown()
    {
        std::lock_guard<std::mutex> lock(shutdown_lock);
        is_shutdown_requested = true;
        wf_server_interrupt(server);
    }

    std::mutex shutdown_lock;
    std::thread thread;
    bool is_shutdown_requested;
    TempDir tempdir;
    wf_server_config * config;
    wf_server * server;
};

Server::Server()
: d(new Server::Private())
{

}

Server::~Server()
{
    delete d;
}

char const * Server::GetBaseDir(void) const
{
    return d->GetBaseDir();
}

int Server::GetPort(void) const
{
    return d->GetPort();
}


}