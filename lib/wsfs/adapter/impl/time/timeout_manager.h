#ifndef WSFS_ADAPTER_IMPL_TIME_TIMEOUT_MANAGER_H
#define WSFS_ADAPTER_IMPL_TIME_TIMEOUT_MANAGER_H

#ifdef __cplusplus
extern "C"
{
#endif

struct timer;
struct timeout_manager
{
    struct timer * timers;
};

extern void timeout_manager_init(
    struct timeout_manager * manager);

extern void timeout_manager_cleanup(
    struct timeout_manager * manager);

extern void timeout_manager_check(
    struct timeout_manager * manager);


#ifdef __cplusplus
}
#endif

#endif
