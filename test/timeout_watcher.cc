#include "timeout_watcher.hpp"
#include <stdexcept>

using std::chrono::milliseconds;
using std::chrono::duration_cast;
using std::chrono::steady_clock;

namespace
{
    milliseconds now()
    {
        return duration_cast<milliseconds>(steady_clock::now().time_since_epoch());
    }
}

namespace webfuse_test
{

TimeoutWatcher::TimeoutWatcher(milliseconds timeout)
: startedAt(now())
, timeout_(timeout)
{

}

TimeoutWatcher::~TimeoutWatcher()
{

}

bool TimeoutWatcher::isTimeout()
{
    return (now() - startedAt) > timeout_;
}

void TimeoutWatcher::check()
{
    if (isTimeout())
    {
        throw std::runtime_error("timeout");
    }
}

}