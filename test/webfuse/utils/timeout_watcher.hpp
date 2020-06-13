#ifndef WF_TEST_TIMEOUT_WATCHER_HPP
#define WF_TEST_TIMEOUT_WATCHER_HPP

#include <chrono>
#include <functional>

namespace webfuse_test
{

class TimeoutWatcher final
{
    TimeoutWatcher(TimeoutWatcher const & other) = delete;
    TimeoutWatcher& operator=(TimeoutWatcher const & other) = delete;
public:
    explicit TimeoutWatcher(std::chrono::milliseconds timeout);
    ~TimeoutWatcher();
    bool isTimeout();
    void check();
    bool waitUntil(std::function<bool()> predicate);
private:
    std::chrono::milliseconds startedAt;
    std::chrono::milliseconds timeout_;
};

}


#endif
