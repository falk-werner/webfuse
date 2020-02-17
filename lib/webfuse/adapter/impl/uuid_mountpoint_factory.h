#ifndef WF_IMPL_UUID_MOUNTPOINT_FACTORY_H
#define WF_IMPL_UUID_MOUNTPOINT_FACTORY_H

#include "webfuse/adapter/impl/mountpoint_factory.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

extern bool
wf_impl_uuid_mountpoint_factory_init(
    struct wf_impl_mountpoint_factory * factory,
    char const * root_path);

#ifdef __cplusplus
}
#endif

#endif
