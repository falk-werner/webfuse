#include "wf/timer.h"

#include "wf/timer/impl/manager.h"
#include "wf/timer/impl/timer.h"

// manager

struct wf_timer_manager * 
wf_timer_manager_create(void)
{
    return wf_timer_impl_manager_create();
}

void
wf_timer_manager_dispose(
    struct wf_timer_manager * manager)
{
    wf_timer_impl_manager_dispose(manager);
}

void
wf_timer_manager_check(
    struct wf_timer_manager * manager)
{
    wf_timer_impl_manager_check(manager);
}

// timer

struct wf_timer *
wf_timer_create(
    struct wf_timer_manager * manager,
    wf_timer_on_timer_fn * on_timer,
    void * user_data)
{
    return wf_timer_impl_create(manager, on_timer, user_data);
}

void
wf_timer_dispose(
    struct wf_timer * timer)
{
    wf_timer_impl_dispose(timer);
}

void
wf_timer_start(
    struct wf_timer * timer,
    int timeout_ms)
{
    wf_timer_impl_start(timer, timeout_ms);
}

void
wf_timer_cancel(
    struct wf_timer * timer)
{
    wf_timer_impl_cancel(timer);
}
