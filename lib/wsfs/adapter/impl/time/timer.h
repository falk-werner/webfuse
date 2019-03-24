#ifndef WSFS_ADAPTER_IMPL_TIME_TIMER_H
#define WSFS_ADAPTER_IMPL_TIME_TIMER_H

#include "wsfs/adapter/impl/time/timepoint.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct timer;
struct timeout_manager;

typedef void timer_timeout_fn(struct timer * timer);

struct timer
{
    struct timeout_manager * manager;
    timepoint timeout;
    timer_timeout_fn * timeout_handler;
    void * user_data;
    struct timer * next;
    struct timer * prev;
};

extern void timer_init(
    struct timer * timer,
    struct timeout_manager * manager);

extern void timer_cleanup(
    struct timer * timer);

extern void timer_start(
    struct timer * timer,
    timepoint absolute_timeout,
    timer_timeout_fn * handler,
    void * user_data);

extern void timer_cancel(
    struct timer * timer);

#ifdef __cplusplus
}
#endif



#endif
