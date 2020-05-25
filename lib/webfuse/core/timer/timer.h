#ifndef WF_TIMER_TIMER_H
#define WF_TIMER_TIMER_H

#include "webfuse/core/timer/on_timer_fn.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_timer;
struct wf_timer_manager;

extern struct wf_timer *
wf_timer_create(
    struct wf_timer_manager * manager,
    wf_timer_on_timer_fn * on_timer,
    void * user_data);

extern void
wf_timer_dispose(
    struct wf_timer * timer);

extern void
wf_timer_start(
    struct wf_timer * timer,
    int timeout_ms);

extern void
wf_timer_cancel(
    struct wf_timer * timer);

#ifdef __cplusplus
}
#endif

#endif
