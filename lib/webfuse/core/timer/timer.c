#include "webfuse/core/timer/timer_intern.h"
#include "webfuse/core/timer/manager_intern.h"
#include "webfuse/core/timer/timepoint.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

struct wf_timer *
wf_timer_create(
    struct wf_timer_manager * manager,
    wf_timer_on_timer_fn * on_timer,
    void * user_data)
{
    struct wf_timer * timer = malloc(sizeof(struct wf_timer));
    timer->manager = manager;
    timer->timeout = 0;
    timer->on_timer = on_timer;
    timer->user_data = user_data;
    timer->prev = NULL;
    timer->next = NULL;

    return timer;
}

void
wf_timer_dispose(
    struct wf_timer * timer)
{
    free(timer);
}

void wf_timer_start(
    struct wf_timer * timer,
    int timeout_ms)
{
    timer->timeout = wf_timer_timepoint_in_msec(timeout_ms);

    wf_timer_manager_addtimer(timer->manager, timer);
}

void wf_timer_cancel(
    struct wf_timer * timer)
{
    wf_timer_manager_removetimer(timer->manager, timer);

    timer->timeout = 0;
}

bool wf_timer_is_timeout(
    struct wf_timer * timer)
{
    return wf_timer_timepoint_is_elapsed(timer->timeout);
}


void wf_timer_trigger(
    struct wf_timer * timer)
{
    if (0 != timer->on_timer)
    {
        timer->prev = NULL;
        timer->next = NULL;

        timer->on_timer(timer, timer->user_data);
    }
}
