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

extern bool timer_is_timeout(
    struct timer * timer);

extern void timer_trigger(
    struct timer * timer);

#ifdef __cplusplus
}
#endif

#endif
