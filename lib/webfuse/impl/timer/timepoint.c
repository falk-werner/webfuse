#include "webfuse/impl/timer/timepoint.h"

#include <time.h>

#define WF_IMPL_TIMER_MSEC_PER_SEC  ((wf_timer_timepoint) 1000) 
#define WF_IMPL_TIMER_NSEC_PER_MSEC ((wf_timer_timepoint) 1000 * 1000) 

wf_timer_timepoint wf_impl_timer_timepoint_now(void)
{
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);

    wf_timer_timepoint const now = (tp.tv_sec * WF_IMPL_TIMER_MSEC_PER_SEC) + (tp.tv_nsec / WF_IMPL_TIMER_NSEC_PER_MSEC);
    return now;
}

wf_timer_timepoint wf_impl_timer_timepoint_in_msec(wf_timer_timediff value)
{
    wf_timer_timepoint const now = wf_impl_timer_timepoint_now();
    wf_timer_timepoint result = now + ((wf_timer_timepoint) value);

    return result;
}

bool wf_impl_timer_timepoint_is_elapsed(wf_timer_timepoint tp)
{
    wf_timer_timepoint const now = wf_impl_timer_timepoint_now();
    wf_timer_timediff const  diff = (wf_timer_timediff) (tp - now);

    return (0 > diff);
}
