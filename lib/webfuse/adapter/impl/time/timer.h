#ifndef WF_ADAPTER_IMPL_TIME_TIMER_H
#define WF_ADAPTER_IMPL_TIME_TIMER_H

#include "webfuse/adapter/impl/time/timepoint.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_impl_timer;
struct wf_impl_timeout_manager;

typedef void wf_impl_timer_timeout_fn(struct wf_impl_timer * timer);

struct wf_impl_timer
{
    struct wf_impl_timeout_manager * manager;
    wf_impl_timepoint timeout;
    wf_impl_timer_timeout_fn * timeout_handler;
    void * user_data;
    struct wf_impl_timer * next;
    struct wf_impl_timer * prev;
};

extern void wf_impl_timer_init(
    struct wf_impl_timer * timer,
    struct wf_impl_timeout_manager * manager);

extern void wf_impl_timer_cleanup(
    struct wf_impl_timer * timer);

extern void wf_impl_timer_start(
    struct wf_impl_timer * timer,
    wf_impl_timepoint absolute_timeout,
    wf_impl_timer_timeout_fn * handler,
    void * user_data);

extern void wf_impl_timer_cancel(
    struct wf_impl_timer * timer);

#ifdef __cplusplus
}
#endif



#endif
