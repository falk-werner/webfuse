#ifndef WEBFUSE_FIXTURE_HPP
#define WEBFUSE_FIXTURE_HPP

#include "webfuse/filesystem/filesystem_i.hpp"
#include "webfuse/webfuse.hpp"
#include "webfuse/provider.hpp"

#include "webfuse/test/tempdir.hpp"
#include "webfuse/test/daemon.hpp"

#include <string>
#include <memory>
#include <thread>
#include <atomic>

namespace webfuse
{

class fixture
{
    fixture(fixture const &) = delete;
    fixture& operator=(fixture const &) = delete;
    fixture(fixture const &&) = delete;
    fixture& operator=(fixture &&) = delete;
public:
    explicit fixture(filesystem_i & fs);
    ~fixture();

    std::string const & get_path() const;

    void reconnect();
private:
    void provider_run();
    std::atomic<bool> shutdown_requested;
    std::atomic<bool> provider_running;
    provider fs_provider;

    tempdir working_dir;
    daemon app;
    std::thread provider_thread;
};


}

#endif
