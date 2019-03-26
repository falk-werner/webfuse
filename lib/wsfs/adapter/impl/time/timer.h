#ifndef WSFS_ADAPTER_IMPL_TIME_TIMER_H
#define WSFS_ADAPTER_IMPL_TIME_TIMER_H

#include "wsfs/adapter/impl/time/timepoint.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wsfs_impl_timer;
struct wsfs_impl_timeout_manager;

typedef void wsfs_impl_timer_timeout_fn(struct wsfs_impl_timer * timer);

struct wsfs_impl_timer
{
    struct wsfs_impl_timeout_manager * manager;
    wsfs_impl_timepoint timeout;
    wsfs_impl_timer_timeout_fn * timeout_handler;
    void * user_data;
    struct wsfs_impl_timer * next;
    struct wsfs_impl_timer * prev;
};

extern void wsfs_impl_timer_init(
    struct wsfs_impl_timer * timer,
    struct wsfs_impl_timeout_manager * manager);

extern void wsfs_impl_timer_cleanup(
    struct wsfs_impl_timer * timer);

extern void wsfs_impl_timer_start(
    struct wsfs_impl_timer * timer,
    wsfs_impl_timepoint absolute_timeout,
    wsfs_impl_timer_timeout_fn * handler,
    void * user_data);

extern void wsfs_impl_timer_cancel(
    struct wsfs_impl_timer * timer);

#ifdef __cplusplus
}
#endif



#endif
