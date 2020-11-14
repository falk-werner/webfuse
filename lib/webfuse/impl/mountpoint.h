#ifndef WF_IMPL_MOUNTPOINT_H
#define WF_IMPL_MOUNTPOINT_H

#include "webfuse/mountpoint.h"

#ifndef __cplusplus
#include <stddef.h>
#else
#include <cstddef>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_mountoptions
{
    char * * items;
    size_t size;
    size_t capacity;
};

struct wf_mountpoint
{
    char * path;
    void * user_data;
    wf_mountpoint_userdata_dispose_fn * dispose;
    struct wf_mountoptions options;
};

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
wf_impl_mountpoint_set_userdata(
    struct wf_mountpoint * mountpoint,
    void * user_data,
    wf_mountpoint_userdata_dispose_fn * dispose);

extern void
wf_impl_mountpoint_add_mountoption(
    struct wf_mountpoint * mountpoint,
    char const * option);

#ifdef __cplusplus
}
#endif

#endif
