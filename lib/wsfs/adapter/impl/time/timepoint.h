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

typedef uint64_t timepoint;
typedef int64_t timediff;

extern timepoint timepoint_now(void);

extern timepoint timepoint_in_msec(
    timediff value);

extern bool timepoint_is_elapsed(
    timepoint timepoint);

#ifdef __cplusplus
} 
#endif

#endif
