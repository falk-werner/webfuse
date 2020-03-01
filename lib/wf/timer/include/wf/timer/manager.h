#ifndef WF_TIMER_MANAGER_H
#define WF_TIMER_MANAGER_H

#include <wf/timer/api.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_timer_manager;

extern WF_TIMER_API struct wf_timer_manager * 
wf_timer_manager_create(void);

extern WF_TIMER_API void
wf_timer_manager_dispose(
    struct wf_timer_manager * manager);

extern WF_TIMER_API void
wf_timer_manager_check(
    struct wf_timer_manager * manager);

#ifdef __cplusplus
}
#endif


#endif
