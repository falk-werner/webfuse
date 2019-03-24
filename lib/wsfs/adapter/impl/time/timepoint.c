#include "wsfs/adapter/impl/time/timepoint.h"

#include <time.h>

#define WSFS_MSEC_PER_SEC  ((timepoint) 1000) 
#define WSFS_NSEC_PER_MSEC ((timepoint) 1000 * 1000) 

timepoint timepoint_now(void)
{
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);

    timepoint const now = (tp.tv_sec * WSFS_MSEC_PER_SEC) + (tp.tv_nsec / WSFS_NSEC_PER_MSEC);
    return now;
}

timepoint timepoint_in_msec(timediff value)
{
    timepoint const now = timepoint_now();
    timepoint result = now + ((timepoint) value);

    return result;
}

bool timepoint_is_elapsed(timepoint tp)
{
    timepoint const now = timepoint_now();
    timediff const  diff = (timediff) (tp - now);

    return (0 > diff);
}
