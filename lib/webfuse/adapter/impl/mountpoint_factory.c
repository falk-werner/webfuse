#include "webfuse/adapter/impl/mountpoint_factory.h"
#include <stddef.h>

void
wf_impl_mountpoint_factory_init_default(
    struct wf_impl_mountpoint_factory * factory)
{
    factory->create_mountpoint = NULL;
    factory->user_data = NULL;
}

void
wf_impl_mountpoint_factory_init(
    struct wf_impl_mountpoint_factory * factory,
    wf_create_mountpoint_fn * create_mountpoint,
    void * user_data)
{
    factory->create_mountpoint = create_mountpoint;
    factory->user_data = user_data;
}

void
wf_impl_mountpoint_factory_clone(
    struct wf_impl_mountpoint_factory * factory,
    struct wf_impl_mountpoint_factory * other)
{
    other->create_mountpoint = factory->create_mountpoint;
    other->user_data = factory->user_data;
}

bool
wf_impl_mountpoint_factory_isvalid(
    struct wf_impl_mountpoint_factory * factory)
{
    return (NULL != factory->create_mountpoint);
}

void
wf_impl_mountpoint_factory_cleanup(
    struct wf_impl_mountpoint_factory * factory)
{

    factory->create_mountpoint = NULL;
    factory->user_data = NULL;
}

struct wf_mountpoint *
wf_impl_mountpoint_factory_create_mountpoint(
    struct wf_impl_mountpoint_factory * factory,
    char const * filesystem)
{
    return factory->create_mountpoint(filesystem, factory->user_data);
}

