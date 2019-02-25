#ifndef WSFS_ADAPTER_TIME_TIMER_INTERN_H
#define WSFS_ADAPTER_TIME_TIMER_INTERN_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include "wsfs/adapter/time/timer.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern bool wsfs_timer_is_timeout(
    struct wsfs_timer * timer);

extern void wsfs_timer_trigger(
    struct wsfs_timer * timer);

#ifdef __cplusplus
}
#endif

#endif
