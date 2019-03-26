#include "wsfs/adapter/impl/time/timer_intern.h"
#include "wsfs/adapter/impl/time/timeout_manager_intern.h"

#include <stddef.h>
#include <string.h>

void wsfs_impl_timer_init(
    struct wsfs_impl_timer * timer,
    struct wsfs_impl_timeout_manager * manager)
{
    timer->manager = manager;
    timer->timeout = 0;
    timer->timeout_handler = NULL;
    timer->user_data = NULL;
    timer->prev = NULL;
    timer->next = NULL;
}

void wsfs_impl_timer_cleanup(
    struct wsfs_impl_timer * timer)
{
    memset(timer, 0, sizeof(struct wsfs_impl_timer));    
}

void wsfs_impl_timer_start(
    struct wsfs_impl_timer * timer,
    wsfs_impl_timepoint absolute_timeout,
    wsfs_impl_timer_timeout_fn * handler,
    void * user_data)
{
    timer->timeout = absolute_timeout;
    timer->timeout_handler = handler;
    timer->user_data = user_data;

    wsfs_impl_timeout_manager_addtimer(timer->manager, timer);
}

void wsfs_impl_timer_cancel(
    struct wsfs_impl_timer * timer)
{
    wsfs_impl_timeout_manager_removetimer(timer->manager, timer);

    timer->timeout = 0;
    timer->timeout_handler = NULL;
    timer->user_data = NULL;
}

bool wsfs_impl_timer_is_timeout(
    struct wsfs_impl_timer * timer)
{
    return wsfs_impl_timepoint_is_elapsed(timer->timeout);
}


void wsfs_impl_timer_trigger(
    struct wsfs_impl_timer * timer)
{
    if (NULL != timer->timeout_handler)
    {
        timer->prev = NULL;
        timer->next = NULL;

        timer->timeout_handler(timer);
    }
}
