#ifndef WF_IMPL_MOUNTPOINT_H
#define WF_IMPL_MOUNTPOINT_H

#include "webfuse/adapter/mountpoint.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern struct wf_mountpoint *
wf_impl_mountpoint_create(
    char const * path);

extern void 
wf_impl_mountpoint_dispose(
    struct wf_mountpoint * mountpoint);

extern char const * 
wf_impl_mountpoint_get_path(
    struct wf_mountpoint const * mountpoint);

extern void
wf_impl_mountpoint_set_ondispose(
    struct wf_mountpoint * mointpoint,
    wf_mountpoint_ondispose_fn * ondispose);

#ifdef __cplusplus
}
#endif

#endif
