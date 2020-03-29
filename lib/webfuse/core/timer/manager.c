#include "webfuse/core/timer/manager_intern.h"
#include "webfuse/core/timer/timer_intern.h"
#include "webfuse/core/timer/timepoint.h"

#include <stddef.h>
#include <stdlib.h>

struct wf_timer_manager
{
    struct wf_timer * timers;
};

struct wf_timer_manager *
wf_timer_manager_create(void)
{
    struct wf_timer_manager * manager = malloc(sizeof(struct wf_timer_manager));
    manager->timers = NULL;

    return manager;
}

void
wf_timer_manager_dispose(
    struct wf_timer_manager * manager)
{
    struct wf_timer * timer = manager->timers;
    while (NULL != timer)
    {
        struct wf_timer * next = timer->next;

        wf_timer_trigger(timer);
        timer = next;
    }

    free(manager);
}


void wf_timer_manager_check(
    struct wf_timer_manager * manager)
{
    struct wf_timer * timer = manager->timers;
    while (NULL != timer)
    {
        struct wf_timer * next = timer->next;

        if (wf_timer_is_timeout(timer))
        {
            wf_timer_manager_removetimer(manager, timer);
            wf_timer_trigger(timer);
        }

        timer = next;
    }    
}

void wf_timer_manager_addtimer(
    struct wf_timer_manager * manager,
    struct wf_timer * timer)
{
    if (NULL != manager->timers)
    {
        manager->timers->prev = timer;
    }

    timer->next = manager->timers;
    timer->prev = NULL;
    manager->timers = timer;
}

void wf_timer_manager_removetimer(
    struct wf_timer_manager * manager,
    struct wf_timer * timer)
{
    struct wf_timer * prev = timer->prev;
    struct wf_timer * next = timer->next;

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

