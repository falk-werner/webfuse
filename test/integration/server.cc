#include "integration/server.hpp"
#include <thread>
#include <mutex>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include "webfuse_adapter.h"
#include "msleep.hpp"

#define WF_PATH_MAX (100)

namespace webfuse_test
{

class Server::Private
{
public:
    Private()
    : is_running(false)
    , is_shutdown_requested(false)
    {
        snprintf(base_dir, WF_PATH_MAX, "%s", "/tmp/webfuse_test_integration_XXXXXX");
        mkdtemp(base_dir);
    }

    ~Private()
    {
        Stop();
        rmdir(base_dir);
    }

    void Start()
    {
        std::lock_guard<std::mutex> lock(run_lock);

        if (!is_running)
        {
            thread = std::thread(Run, this);
            is_running = true;
            msleep(200);
        }
    }

    void Stop()
    {
        std::lock_guard<std::mutex> lock(run_lock);

        if (is_running)
        {
            RequestShutdown();
            thread.join();
            is_running = false;
        }
    }

    bool IsShutdownRequested()
    {
        std::lock_guard<std::mutex> lock(shutdown_lock);
        return is_shutdown_requested;
    }

    char const * GetBaseDir()
    {
        return base_dir;
    }

private:
    void RequestShutdown()
    {
        std::lock_guard<std::mutex> lock(shutdown_lock);
        is_shutdown_requested = true;
    }

    static void Run(Server::Private * context)
    {
        wf_server_config * config = wf_server_config_create();
        wf_server_config_set_port(config, 8080);
        wf_server_config_set_mountpoint(config, context->GetBaseDir());

        wf_server * server = wf_server_create(config);
        if (nullptr != server)
        {
            while (!context->IsShutdownRequested())
            {
                wf_server_service(server, 100);
            }

            wf_server_dispose(server);
        }

        wf_server_config_dispose(config);
    }

    char base_dir[WF_PATH_MAX];

    bool is_running;
    std::mutex run_lock;

    bool is_shutdown_requested;
    std::mutex shutdown_lock;

    std::thread thread;
};

Server::Server()
: d(new Server::Private())
{

}

Server::~Server()
{
    delete d;
}

void Server::Start(void)
{
    d->Start();
}

void Server::Stop(void)
{
    d->Stop();
}

char const * Server::GetBaseDir(void) const
{
    return d->GetBaseDir();
}


}