#ifndef WEBFUSE_FILETIME_HPP
#define WEBFUSE_FILETIME_HPP

#include <ctime>
#include <cinttypes>

namespace webfuse
{

class filetime
{
public:
    filetime();
    static filetime from_timespec(timespec const & other);
    static filetime from_time(time_t const & other);
    void to_timespec(timespec & other) const;
    time_t to_time() const;

    uint64_t seconds;
    uint32_t nsec;
};

}

#endif
