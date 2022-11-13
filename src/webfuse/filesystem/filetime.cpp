#include "webfuse/filesystem/filetime.hpp"

namespace webfuse
{

filetime::filetime()
: seconds(0)
, nsec(0)
{

}

filetime filetime::from_timespec(timespec const & other)
{
    filetime result;
    result.seconds = static_cast<uint64_t>(other.tv_sec);
    result.nsec = static_cast<uint32_t>(other.tv_nsec);

    return result;
}

filetime filetime::from_time(time_t const & other)
{
    filetime result;
    result.seconds = static_cast<uint64_t>(other);
    result.nsec = 0;

    return result;
}

void filetime::to_timespec(timespec & other) const
{
    other.tv_sec = seconds;
    other.tv_nsec = static_cast<time_t>(nsec);
}

time_t filetime::to_time() const
{
    return static_cast<time_t>(seconds);
}


}