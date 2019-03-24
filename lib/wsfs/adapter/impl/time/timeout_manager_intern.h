#ifndef WSFS_ADAPTER_IMPL_TIME_TIMEOUT_MANAGER_INTERN_H
#define WSFS_ADAPTER_IMPL_TIME_TIMEOUT_MANAGER_INTERN_H

#include "wsfs/adapter/impl/time/timeout_manager.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void timeout_manager_addtimer(
    struct timeout_manager * manager,
    struct timer * timer);

extern void timeout_manager_removetimer(
    struct timeout_manager * manager,
    struct timer * timer);

#ifdef __cplusplus
}
#endif


#endif
