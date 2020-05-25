#ifndef WF_ADAPTER_IMPL_MOUNTPOINT_FACTORY_H
#define WF_ADAPTER_IMPL_MOUNTPOINT_FACTORY_H

#include "webfuse/adapter/mountpoint_factory.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_impl_mountpoint_factory
{
    wf_create_mountpoint_fn * create_mountpoint;
    void * user_data;
};

extern void
wf_impl_mountpoint_factory_init_default(
    struct wf_impl_mountpoint_factory * factory);

extern void
wf_impl_mountpoint_factory_init(
    struct wf_impl_mountpoint_factory * factory,
    wf_create_mountpoint_fn * create_mountpoint,
    void * user_data);

extern void
wf_impl_mountpoint_factory_clone(
    struct wf_impl_mountpoint_factory * factory,
    struct wf_impl_mountpoint_factory * other);
    
extern bool
wf_impl_mountpoint_factory_isvalid(
    struct wf_impl_mountpoint_factory * factory);

extern void
wf_impl_mountpoint_factory_init_from(
    struct wf_impl_mountpoint_factory * factory,
    struct wf_impl_mountpoint_factory * other);

extern void
wf_impl_mountpoint_factory_cleanup(
    struct wf_impl_mountpoint_factory * factory);

extern struct wf_mountpoint *
wf_impl_mountpoint_factory_create_mountpoint(
    struct wf_impl_mountpoint_factory * factory,
    char const * filesystem);

#ifdef __cplusplus
}
#endif

#endif
