#include "wsfs/adapter/impl/time/timer_intern.h"
#include "wsfs/adapter/impl/time/timeout_manager_intern.h"

#include <stddef.h>
#include <string.h>

void timer_init(
    struct timer * timer,
    struct timeout_manager * manager)
{
    timer->manager = manager;
    timer->timeout = 0;
    timer->timeout_handler = NULL;
    timer->user_data = NULL;
    timer->prev = NULL;
    timer->next = NULL;
}

void timer_cleanup(
    struct timer * timer)
{
    memset(timer, 0, sizeof(struct timer));    
}

void timer_start(
    struct timer * timer,
    timepoint absolute_timeout,
    timer_timeout_fn * handler,
    void * user_data)
{
    timer->timeout = absolute_timeout;
    timer->timeout_handler = handler;
    timer->user_data = user_data;

    timeout_manager_addtimer(timer->manager, timer);
}

void timer_cancel(
    struct timer * timer)
{
    timeout_manager_removetimer(timer->manager, timer);

    timer->timeout = 0;
    timer->timeout_handler = NULL;
    timer->user_data = NULL;
}

bool timer_is_timeout(
    struct timer * timer)
{
    return timepoint_is_elapsed(timer->timeout);
}


void timer_trigger(
    struct timer * timer)
{
    if (NULL != timer->timeout_handler)
    {
        timer->prev = NULL;
        timer->next = NULL;

        timer->timeout_handler(timer);
    }
}
