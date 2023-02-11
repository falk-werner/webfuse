#include "webfuse/test/fixture.hpp"
#include <csignal>
#include <cstring>
#include <chrono>
#include <iostream>

namespace webfuse
{

fixture::fixture(filesystem_i & fs)
: shutdown_requested(false)
, provider_running(false)
, fs_provider(fs, "")
, app(working_dir.name())
{
    fs_provider.set_connection_listener([this](bool is_connected) {
        if (is_connected)
        {
            this->provider_running = true;
        }

        if ((!is_connected) && (!this->provider_running))
        {
            this->reconnect();
        }
    });
    provider_thread = std::thread(std::bind(&fixture::provider_run, this));
    while (!provider_running)
    {
        std::this_thread::yield();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

fixture::~fixture()
{
    shutdown_requested = true;
    fs_provider.interrupt();
    provider_thread.join();
}

void fixture::provider_run()
{
    fs_provider.connect("ws://localhost:8081/");
    while (!shutdown_requested)
    {
        fs_provider.service();
    }
}

void fixture::reconnect()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    fs_provider.connect("ws://localhost:8081/");
}

std::string const & fixture::get_path() const
{
    return working_dir.name();
}


}
