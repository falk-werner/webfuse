#include "wsfs/adapter/impl/time/timepoint.h"

#include <time.h>

#define WSFS_MSEC_PER_SEC  ((wsfs_impl_timepoint) 1000) 
#define WSFS_NSEC_PER_MSEC ((wsfs_impl_timepoint) 1000 * 1000) 

wsfs_impl_timepoint wsfs_impl_timepoint_now(void)
{
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);

    wsfs_impl_timepoint const now = (tp.tv_sec * WSFS_MSEC_PER_SEC) + (tp.tv_nsec / WSFS_NSEC_PER_MSEC);
    return now;
}

wsfs_impl_timepoint wsfs_impl_timepoint_in_msec(wsfs_impl_timediff value)
{
    wsfs_impl_timepoint const now = wsfs_impl_timepoint_now();
    wsfs_impl_timepoint result = now + ((wsfs_impl_timepoint) value);

    return result;
}

bool wsfs_impl_timepoint_is_elapsed(wsfs_impl_timepoint tp)
{
    wsfs_impl_timepoint const now = wsfs_impl_timepoint_now();
    wsfs_impl_timediff const  diff = (wsfs_impl_timediff) (tp - now);

    return (0 > diff);
}
