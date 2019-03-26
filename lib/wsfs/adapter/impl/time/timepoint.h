#ifndef WSFS_ADAPTER_IMPL_TIME_TIMEPOINT_H
#define WSFS_ADAPTER_IMPL_TIME_TIMEPOINT_H

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

typedef uint64_t wsfs_impl_timepoint;
typedef int64_t wsfs_impl_timediff;

extern wsfs_impl_timepoint wsfs_impl_timepoint_now(void);

extern wsfs_impl_timepoint wsfs_impl_timepoint_in_msec(
    wsfs_impl_timediff value);

extern bool wsfs_impl_timepoint_is_elapsed(
    wsfs_impl_timepoint timepoint);

#ifdef __cplusplus
} 
#endif

#endif
