#ifndef WSFS_ADAPTER_IMPL_TIME_TIMEOUT_MANAGER_H
#define WSFS_ADAPTER_IMPL_TIME_TIMEOUT_MANAGER_H

#ifdef __cplusplus
extern "C"
{
#endif

struct wsfs_timer;
struct wsfs_timeout_manager
{
    struct wsfs_timer * timers;
};

extern void wsfs_timeout_manager_init(
    struct wsfs_timeout_manager * manager);

extern void wsfs_timeout_manager_cleanup(
    struct wsfs_timeout_manager * manager);

extern void wsfs_timeout_manager_check(
    struct wsfs_timeout_manager * manager);


#ifdef __cplusplus
}
#endif

#endif
