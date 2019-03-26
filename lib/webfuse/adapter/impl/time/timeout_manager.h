#ifndef WF_ADAPTER_IMPL_TIME_TIMEOUT_MANAGER_H
#define WF_ADAPTER_IMPL_TIME_TIMEOUT_MANAGER_H

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_impl_timer;
struct wf_impl_timeout_manager
{
    struct wf_impl_timer * timers;
};

extern void wf_impl_timeout_manager_init(
    struct wf_impl_timeout_manager * manager);

extern void wf_impl_timeout_manager_cleanup(
    struct wf_impl_timeout_manager * manager);

extern void wf_impl_timeout_manager_check(
    struct wf_impl_timeout_manager * manager);


#ifdef __cplusplus
}
#endif

#endif
