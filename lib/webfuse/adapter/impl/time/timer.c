#include "webfuse/adapter/impl/time/timer_intern.h"
#include "webfuse/adapter/impl/time/timeout_manager_intern.h"

#include <stddef.h>
#include <string.h>

void wf_impl_timer_init(
    struct wf_impl_timer * timer,
    struct wf_impl_timeout_manager * manager)
{
    timer->manager = manager;
    timer->timeout = 0;
    timer->timeout_handler = NULL;
    timer->user_data = NULL;
    timer->prev = NULL;
    timer->next = NULL;
}

void wf_impl_timer_cleanup(
    struct wf_impl_timer * timer)
{
    memset(timer, 0, sizeof(struct wf_impl_timer));    
}

void wf_impl_timer_start(
    struct wf_impl_timer * timer,
    wf_impl_timepoint absolute_timeout,
    wf_impl_timer_timeout_fn * handler,
    void * user_data)
{
    timer->timeout = absolute_timeout;
    timer->timeout_handler = handler;
    timer->user_data = user_data;

    wf_impl_timeout_manager_addtimer(timer->manager, timer);
}

void wf_impl_timer_cancel(
    struct wf_impl_timer * timer)
{
    wf_impl_timeout_manager_removetimer(timer->manager, timer);

    timer->timeout = 0;
    timer->timeout_handler = NULL;
    timer->user_data = NULL;
}

bool wf_impl_timer_is_timeout(
    struct wf_impl_timer * timer)
{
    return wf_impl_timepoint_is_elapsed(timer->timeout);
}


void wf_impl_timer_trigger(
    struct wf_impl_timer * timer)
{
    if (NULL != timer->timeout_handler)
    {
        timer->prev = NULL;
        timer->next = NULL;

        timer->timeout_handler(timer);
    }
}
