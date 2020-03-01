#include "webfuse/tests/integration/server.hpp"
#include <thread>
#include <mutex>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include "webfuse_adapter.h"
#include "webfuse/adapter/impl/server.h"

#define WF_PATH_MAX (100)

namespace webfuse_test
{

class Server::Private
{
public:
    Private()
    : is_shutdown_requested(false)
    {
        snprintf(base_dir, WF_PATH_MAX, "%s", "/tmp/webfuse_test_integration_XXXXXX");
        char const * result = mkdtemp(base_dir);
        if (NULL == result)
        {
            throw std::runtime_error("unable to create temp dir");
        }


        config = wf_server_config_create();
        wf_server_config_set_port(config, 8080);
        wf_server_config_set_mountpoint(config, base_dir);

        server = wf_server_create(config);

        while (!wf_impl_server_is_operational(server))
        {
            wf_server_service(server, 100);
        }

        thread = std::thread(Run, this);

    }

    ~Private()
    {
        RequestShutdown();
        thread.join();
        rmdir(base_dir);
        wf_server_dispose(server);
        wf_server_config_dispose(config);
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
    }

    static void Run(Server::Private * context)
    {
        while (!context->IsShutdownRequested())
        {
            wf_server_service(context->server, 100);
        }
    }


    std::mutex shutdown_lock;
    std::thread thread;
    bool is_shutdown_requested;


public:
    char base_dir[WF_PATH_MAX];
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
    return d->base_dir;
}


}