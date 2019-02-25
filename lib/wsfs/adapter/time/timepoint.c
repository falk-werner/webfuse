#include "wsfs/adapter/time/timepoint.h"

#include <time.h>

#define WSFS_MSEC_PER_SEC  ((wsfs_timepoint) 1000) 
#define WSFS_NSEC_PER_MSEC ((wsfs_timepoint) 1000 * 1000) 

wsfs_timepoint wsfs_timepoint_now(void)
{
    struct timespec timepoint;
    clock_gettime(CLOCK_MONOTONIC, &timepoint);

    wsfs_timepoint const now = (timepoint.tv_sec * WSFS_MSEC_PER_SEC) + (timepoint.tv_nsec / WSFS_NSEC_PER_MSEC);
    return now;
}

wsfs_timepoint wsfs_timepoint_in_msec(wsfs_timediff value)
{
    wsfs_timepoint const now = wsfs_timepoint_now();
    wsfs_timepoint result = now + ((wsfs_timepoint) value);

    return result;
}

bool wsfs_timepoint_is_elapsed(wsfs_timepoint timepoint)
{
    wsfs_timepoint const now = wsfs_timepoint_now();
    wsfs_timediff const  diff = (wsfs_timediff) (timepoint - now);

    return (0 > diff);
}
