#ifndef WF_TIMER_IMPL_MANAGER_H
#define WF_TIMER_IMPL_MANAGER_H

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_timer;
struct wf_timer_manager;

extern struct wf_timer_manager *
wf_timer_impl_manager_create(void);

extern void
wf_timer_impl_manager_dispose(
    struct wf_timer_manager * manager);

extern void
wf_timer_impl_manager_check(
    struct wf_timer_manager * manager);

extern void wf_timer_impl_manager_addtimer(
    struct wf_timer_manager * manager,
    struct wf_timer * timer);

extern void wf_timer_impl_manager_removetimer(
    struct wf_timer_manager * manager,
    struct wf_timer * timer);

#ifdef __cplusplus
}
#endif


#endif
