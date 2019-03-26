#include "webfuse/adapter/impl/time/timeout_manager_intern.h"

#include <stddef.h>
#include "webfuse/adapter/impl/time/timer_intern.h"
#include "webfuse/adapter/impl/time/timepoint.h"

void wf_impl_timeout_manager_init(
    struct wf_impl_timeout_manager * manager)
{
    manager->timers = NULL;
}

void wf_impl_timeout_manager_cleanup(
    struct wf_impl_timeout_manager * manager)
{
    struct wf_impl_timer * timer = manager->timers;
    while (NULL != timer)
    {
        struct wf_impl_timer * next = timer->next;

        wf_impl_timer_trigger(timer);

        timer = next;
    }

    manager->timers = NULL;

}

void wf_impl_timeout_manager_check(
    struct wf_impl_timeout_manager * manager)
{
    struct wf_impl_timer * timer = manager->timers;
    while (NULL != timer)
    {
        struct wf_impl_timer * next = timer->next;

        if (wf_impl_timer_is_timeout(timer))
        {
            wf_impl_timeout_manager_removetimer(manager, timer);
            wf_impl_timer_trigger(timer);
        }

        timer = next;
    }    
}

void wf_impl_timeout_manager_addtimer(
    struct wf_impl_timeout_manager * manager,
    struct wf_impl_timer * timer)
{
    if (NULL != manager->timers)
    {
        manager->timers->prev = timer;
    }

    timer->next = manager->timers;
    timer->prev = NULL;
    manager->timers = timer;
}

void wf_impl_timeout_manager_removetimer(
    struct wf_impl_timeout_manager * manager,
    struct wf_impl_timer * timer)
{
    struct wf_impl_timer * prev = timer->prev;
    struct wf_impl_timer * next = timer->next;

    if (NULL != prev)
    {
        prev->next = next;
    }

    if (NULL != next)
    {
        next->prev = prev;
    }

    if (manager->timers == timer)
    {
        manager->timers = next;
    }
}

