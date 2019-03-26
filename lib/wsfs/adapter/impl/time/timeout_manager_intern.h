#ifndef WSFS_ADAPTER_IMPL_TIME_TIMEOUT_MANAGER_INTERN_H
#define WSFS_ADAPTER_IMPL_TIME_TIMEOUT_MANAGER_INTERN_H

#include "wsfs/adapter/impl/time/timeout_manager.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void wsfs_impl_timeout_manager_addtimer(
    struct wsfs_impl_timeout_manager * manager,
    struct wsfs_impl_timer * timer);

extern void wsfs_impl_timeout_manager_removetimer(
    struct wsfs_impl_timeout_manager * manager,
    struct wsfs_impl_timer * timer);

#ifdef __cplusplus
}
#endif


#endif
