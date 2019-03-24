#ifndef WSFS_ADAPTER_IMPL_TIME_TIMER_INTERN_H
#define WSFS_ADAPTER_IMPL_TIME_TIMER_INTERN_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include "wsfs/adapter/impl/time/timer.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern bool wsfs_impl_timer_is_timeout(
    struct wsfs_impl_timer * timer);

extern void wsfs_impl_timer_trigger(
    struct wsfs_impl_timer * timer);

#ifdef __cplusplus
}
#endif

#endif
