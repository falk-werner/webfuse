#ifndef WF_IMPL_TIMER_TIMER_H
#define WF_IMPL_TIMER_TIMER_H

#include "webfuse/impl/timer/timer.h"
#include "webfuse/impl/timer/on_timer_fn.h"
#include "webfuse/impl/timer/timepoint.h"

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_timer
{
    struct wf_timer_manager * manager;
    wf_timer_timepoint timeout;
    wf_timer_on_timer_fn * on_timer;
    void * user_data;
    struct wf_timer * next;
    struct wf_timer * prev;
};

extern bool wf_impl_timer_is_timeout(
    struct wf_timer * timer);

extern void wf_impl_timer_trigger(
    struct wf_timer * timer);


#ifdef __cplusplus
}
#endif



#endif
