#include "wsfs/adapter/impl/time/timeout_manager_intern.h"

#include <stddef.h>
#include "wsfs/adapter/impl/time/timer_intern.h"
#include "wsfs/adapter/impl/time/timepoint.h"

void timeout_manager_init(
    struct timeout_manager * manager)
{
    manager->timers = NULL;
}

void timeout_manager_cleanup(
    struct timeout_manager * manager)
{
    struct timer * timer = manager->timers;
    while (NULL != timer)
    {
        struct timer * next = timer->next;

        timer_trigger(timer);

        timer = next;
    }

    manager->timers = NULL;

}

void timeout_manager_check(
    struct timeout_manager * manager)
{
    struct timer * timer = manager->timers;
    while (NULL != timer)
    {
        struct timer * next = timer->next;

        if (timer_is_timeout(timer))
        {
            timeout_manager_removetimer(manager, timer);
            timer_trigger(timer);
        }

        timer = next;
    }    
}

void timeout_manager_addtimer(
    struct timeout_manager * manager,
    struct timer * timer)
{
    if (NULL != manager->timers)
    {
        manager->timers->prev = timer;
    }

    timer->next = manager->timers;
    timer->prev = NULL;
    manager->timers = timer;
}

void timeout_manager_removetimer(
    struct timeout_manager * manager,
    struct timer * timer)
{
    struct timer * prev = timer->prev;
    struct timer * next = timer->next;

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

