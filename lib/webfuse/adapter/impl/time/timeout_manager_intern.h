#ifndef WF_ADAPTER_IMPL_TIME_TIMEOUT_MANAGER_INTERN_H
#define WF_ADAPTER_IMPL_TIME_TIMEOUT_MANAGER_INTERN_H

#include "webfuse/adapter/impl/time/timeout_manager.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void wf_impl_timeout_manager_addtimer(
    struct wf_impl_timeout_manager * manager,
    struct wf_impl_timer * timer);

extern void wf_impl_timeout_manager_removetimer(
    struct wf_impl_timeout_manager * manager,
    struct wf_impl_timer * timer);

#ifdef __cplusplus
}
#endif


#endif
