#ifndef WF_TIMER_MANAGER_INTERN_H
#define WF_TIMER_MANAGER_INTERN_H

#include "webfuse/core/timer/manager.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_timer;

extern void wf_timer_manager_addtimer(
    struct wf_timer_manager * manager,
    struct wf_timer * timer);

extern void wf_timer_manager_removetimer(
    struct wf_timer_manager * manager,
    struct wf_timer * timer);

#ifdef __cplusplus
}
#endif


#endif
