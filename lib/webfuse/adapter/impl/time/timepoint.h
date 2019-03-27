#ifndef WF_ADAPTER_IMPL_TIME_TIMEPOINT_H
#define WF_ADAPTER_IMPL_TIME_TIMEPOINT_H

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

typedef uint64_t wf_impl_timepoint;
typedef int64_t wf_impl_timediff;

extern wf_impl_timepoint wf_impl_timepoint_now(void);

extern wf_impl_timepoint wf_impl_timepoint_in_msec(
    wf_impl_timediff value);

extern bool wf_impl_timepoint_is_elapsed(
    wf_impl_timepoint timepoint);

#ifdef __cplusplus
} 
#endif

#endif
