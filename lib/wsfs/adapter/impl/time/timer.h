#ifndef WSFS_ADAPTER_TIME_TIMER_H
#define WSFS_ADAPTER_TIME_TIMER_H

#include "wsfs/adapter/impl/time/timepoint.h"

struct wsfs_timer;
struct wsfs_timeout_manager;

typedef void wsfs_timer_timeout_fn(struct wsfs_timer * timer);

struct wsfs_timer
{
    struct wsfs_timeout_manager * manager;
    wsfs_timepoint timeout;
    wsfs_timer_timeout_fn * timeout_handler;
    void * user_data;
    struct wsfs_timer * next;
    struct wsfs_timer * prev;
};

#ifdef __cplusplus
extern "C"
{
#endif

extern void wsfs_timer_init(
    struct wsfs_timer * timer,
    struct wsfs_timeout_manager * manager);

extern void wsfs_timer_cleanup(
    struct wsfs_timer * timer);

extern void wsfs_timer_start(
    struct wsfs_timer * timer,
    wsfs_timepoint absolute_timeout,
    wsfs_timer_timeout_fn * handler,
    void * user_data);

extern void wsfs_timer_cancel(
    struct wsfs_timer * timer);

#ifdef __cplusplus
}
#endif



#endif
