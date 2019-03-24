#ifndef WSFS_ADAPTER_IMPL_TIME_TIMEPOINT_H
#define WSFS_ADAPTER_IMPL_TIME_TIMEPOINT_H

#ifndef __cplusplus
#include <stdbool.h>
#include <inttypes.h>
#else
#include <cinttypes>
#endif

typedef uint64_t wsfs_timepoint;
typedef int64_t wsfs_timediff;

#ifdef __cplusplus
extern "C"
{
#endif

extern wsfs_timepoint wsfs_timepoint_now(void);

extern wsfs_timepoint wsfs_timepoint_in_msec(
    wsfs_timediff value);

extern bool wsfs_timepoint_is_elapsed(
    wsfs_timepoint timepoint);

#ifdef __cplusplus
} 
#endif

#endif
