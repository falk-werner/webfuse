#include "integration/provider.hpp"
#include "webfuse_provider.h"
#include <thread>
#include <mutex>
#include <string>
#include "msleep.hpp"

namespace webfuse_test
{

class Provider::Private
{
public:
    Private(char const * url_)
    : url(url_)
    , is_running(false)
    , is_shutdown_requested(false)
    {
    }

    ~Private()
    {
        Stop();
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

    char const * GetUrl() const
    {
        return url.c_str();
    }
private:
    void RequestShutdown()
    {
        std::lock_guard<std::mutex> lock(shutdown_lock);
        is_shutdown_requested = true;
    }

    static void Run(Provider::Private * context)
    {
        wfp_client_config * config = wfp_client_config_create();
        wfp_static_filesystem * fs = wfp_static_filesystem_create(config);
        wfp_static_filesystem_add_text(fs, "hello.txt", 0x444, "Hello, World");

        wfp_client * client = wfp_client_create(config);
        if (nullptr != client)
        {
            wfp_client_connect(client, context->GetUrl());

            while (!context->IsShutdownRequested())
            {
                wfp_client_service(client, 100);
            }

            wfp_client_dispose(client);
        }

        wfp_static_filesystem_dispose(fs);
        wfp_client_config_dispose(config);
    }

    std::string url;

    bool is_running;
    std::mutex run_lock;

    bool is_shutdown_requested;
    std::mutex shutdown_lock;

    std::thread thread;
};

Provider::Provider(char const * url)
: d(new Provider::Private(url))
{
}

Provider::~Provider()
{
    delete d;
}

void Provider::Start(void)
{
    d->Start();
}

void Provider::Stop(void)
{
    d->Stop();
}

}