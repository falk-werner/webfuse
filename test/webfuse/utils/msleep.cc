#include "webfuse/utils/msleep.hpp"
#include <ctime>

namespace webfuse_test
{

void msleep(long millis)
{
    long const secs_per_msec = 1000;
    long const msecs_per_nsec = (1000 * 1000);

    long const seconds = millis / secs_per_msec;
    long const nanos = (millis % secs_per_msec) * msecs_per_nsec;

    struct timespec timeout = { seconds, nanos };
    while (0 != nanosleep(&timeout, &timeout));
}

}