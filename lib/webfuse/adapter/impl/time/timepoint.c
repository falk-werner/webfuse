#include "webfuse/adapter/impl/time/timepoint.h"

#include <time.h>

#define WF_MSEC_PER_SEC  ((wf_impl_timepoint) 1000) 
#define WF_NSEC_PER_MSEC ((wf_impl_timepoint) 1000 * 1000) 

wf_impl_timepoint wf_impl_timepoint_now(void)
{
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);

    wf_impl_timepoint const now = (tp.tv_sec * WF_MSEC_PER_SEC) + (tp.tv_nsec / WF_NSEC_PER_MSEC);
    return now;
}

wf_impl_timepoint wf_impl_timepoint_in_msec(wf_impl_timediff value)
{
    wf_impl_timepoint const now = wf_impl_timepoint_now();
    wf_impl_timepoint result = now + ((wf_impl_timepoint) value);

    return result;
}

bool wf_impl_timepoint_is_elapsed(wf_impl_timepoint tp)
{
    wf_impl_timepoint const now = wf_impl_timepoint_now();
    wf_impl_timediff const  diff = (wf_impl_timediff) (tp - now);

    return (0 > diff);
}
