#ifndef WSFS_TIME_TIMEOUT_MANAGER_INTERN_H
#define WSFS_TIME_TIMEOUT_MANAGER_INTERN_H

#include "wsfs/adapter/time/timeout_manager.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void wsfs_timeout_manager_addtimer(
    struct wsfs_timeout_manager * manager,
    struct wsfs_timer * timer);

extern void wsfs_timeout_manager_removetimer(
    struct wsfs_timeout_manager * manager,
    struct wsfs_timer * timer);

#ifdef __cplusplus
}
#endif


#endif
