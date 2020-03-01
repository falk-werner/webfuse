#ifndef WF_ADAPTER_IMPL_TIME_TIMER_H
#define WF_ADAPTER_IMPL_TIME_TIMER_H

#include "wf/timer/on_timer_fn.h"
#include "wf/timer/impl/timepoint.h"

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_timer_manager;

struct wf_timer
{
    struct wf_timer_manager * manager;
    wf_timer_timepoint timeout;
    wf_timer_on_timer_fn * on_timer;
    void * user_data;
    struct wf_timer * next;
    struct wf_timer * prev;
};


struct wf_timer *
wf_timer_impl_create(
    struct wf_timer_manager * manager,
    wf_timer_on_timer_fn * on_timer,
    void * user_data);

void
wf_timer_impl_dispose(
    struct wf_timer * timer);

extern void wf_timer_impl_start(
    struct wf_timer * timer,
    int timeout_ms);

extern void wf_timer_impl_cancel(
    struct wf_timer * timer);

extern bool wf_timer_impl_is_timeout(
    struct wf_timer * timer);

extern void wf_timer_impl_trigger(
    struct wf_timer * timer);


#ifdef __cplusplus
}
#endif



#endif
