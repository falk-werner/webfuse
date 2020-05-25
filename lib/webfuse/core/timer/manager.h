#ifndef WF_TIMER_MANAGER_H
#define WF_TIMER_MANAGER_H

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_timer_manager;

extern struct wf_timer_manager * 
wf_timer_manager_create(void);

extern void
wf_timer_manager_dispose(
    struct wf_timer_manager * manager);

extern void
wf_timer_manager_check(
    struct wf_timer_manager * manager);

#ifdef __cplusplus
}
#endif


#endif
