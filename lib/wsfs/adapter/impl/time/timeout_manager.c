#include "wsfs/adapter/impl/time/timeout_manager_intern.h"

#include <stddef.h>
#include "wsfs/adapter/impl/time/timer_intern.h"
#include "wsfs/adapter/impl/time/timepoint.h"

void wsfs_timeout_manager_init(
    struct wsfs_timeout_manager * manager)
{
    manager->timers = NULL;
}

void wsfs_timeout_manager_cleanup(
    struct wsfs_timeout_manager * manager)
{
    struct wsfs_timer * timer = manager->timers;
    while (NULL != timer)
    {
        struct wsfs_timer * next = timer->next;

        wsfs_timer_trigger(timer);

        timer = next;
    }

    manager->timers = NULL;

}

void wsfs_timeout_manager_check(
    struct wsfs_timeout_manager * manager)
{
    struct wsfs_timer * timer = manager->timers;
    while (NULL != timer)
    {
        struct wsfs_timer * next = timer->next;

        if (wsfs_timer_is_timeout(timer))
        {
            wsfs_timeout_manager_removetimer(manager, timer);
            wsfs_timer_trigger(timer);
        }

        timer = next;
    }    
}

void wsfs_timeout_manager_addtimer(
    struct wsfs_timeout_manager * manager,
    struct wsfs_timer * timer)
{
    if (NULL != manager->timers)
    {
        manager->timers->prev = timer;
    }

    timer->next = manager->timers;
    timer->prev = NULL;
    manager->timers = timer;
}

void wsfs_timeout_manager_removetimer(
    struct wsfs_timeout_manager * manager,
    struct wsfs_timer * timer)
{
    struct wsfs_timer * prev = timer->prev;
    struct wsfs_timer * next = timer->next;

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

