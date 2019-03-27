#ifndef WF_ADAPTER_IMPL_TIME_TIMER_INTERN_H
#define WF_ADAPTER_IMPL_TIME_TIMER_INTERN_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include "webfuse/adapter/impl/time/timer.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern bool wf_impl_timer_is_timeout(
    struct wf_impl_timer * timer);

extern void wf_impl_timer_trigger(
    struct wf_impl_timer * timer);

#ifdef __cplusplus
}
#endif

#endif
