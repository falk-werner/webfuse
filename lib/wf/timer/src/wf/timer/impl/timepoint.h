#ifndef WF_TIMER_IMPL_TIMEPOINT_H
#define WF_TIMER_IMPL_TIMEPOINT_H

#ifndef __cplusplus
#include <stdbool.h>
#include <inttypes.h>
#else
#include <cinttypes>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

typedef uint64_t wf_timer_timepoint;
typedef int64_t wf_timer_timediff;

extern wf_timer_timepoint wf_timer_impl_timepoint_now(void);

extern wf_timer_timepoint wf_timer_impl_timepoint_in_msec(
    wf_timer_timediff value);

extern bool wf_timer_impl_timepoint_is_elapsed(
    wf_timer_timepoint timepoint);

#ifdef __cplusplus
} 
#endif

#endif
