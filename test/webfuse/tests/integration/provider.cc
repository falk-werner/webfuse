#include "webfuse/tests/integration/provider.hpp"
#include "webfuse_provider.h"
#include "webfuse/provider/impl/client.h"
#include <thread>
#include <mutex>
#include <string>
#include "webfuse/utils/msleep.hpp"
#include "webfuse/utils/static_filesystem.h"

namespace webfuse_test
{

class Provider::Private
{
public:
    explicit Private(char const * url)
    : is_shutdown_requested(false)
    {
        config = wfp_client_config_create();

        fs = wfp_static_filesystem_create(config);
        wfp_static_filesystem_add_text(fs, "hello.txt", 0444, "Hello, World");

        client = wfp_client_create(config);
        wfp_client_connect(client, url);
        while (!wfp_impl_client_is_connected(client))
        {
            wfp_client_service(client, 100);
        }
       
        thread = std::thread(Run, this);
        webfuse_test::msleep(200);
    }

    ~Private()
    {
        RequestShutdown();
        thread.join();

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
    }

    static void Run(Provider::Private * context)
    {
        while (!context->IsShutdownRequested())
        {
            wfp_client_service(context->client, 100);
        }
    }

    std::mutex shutdown_lock;
    std::thread thread;
    bool is_shutdown_requested;

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