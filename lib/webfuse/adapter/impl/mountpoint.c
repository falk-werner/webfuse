#include "webfuse/adapter/impl/mountpoint.h"

#include <stdlib.h>
#include <string.h>

struct wf_mountpoint
{
    char * path;
    void * user_data;
    wf_mountpoint_userdata_dispose_fn * dispose;
};

struct wf_mountpoint *
wf_impl_mountpoint_create(
    char const * path)
{
    struct wf_mountpoint * mountpoint = malloc(sizeof(struct wf_mountpoint));
    mountpoint->path = strdup(path);
    mountpoint->user_data = NULL;
    mountpoint->dispose = NULL;

    return mountpoint;
}

void 
wf_impl_mountpoint_dispose(
    struct wf_mountpoint * mountpoint)
{
    if (NULL != mountpoint->dispose)
    {
        mountpoint->dispose(mountpoint->user_data);
    }

    free(mountpoint->path);
    free(mountpoint);
}

char const * 
wf_impl_mountpoint_get_path(
    struct wf_mountpoint const * mountpoint)
{
    return mountpoint->path;
}

extern void
wf_impl_mountpoint_set_userdata(
    struct wf_mountpoint * mountpoint,
    void * user_data,
    wf_mountpoint_userdata_dispose_fn * dispose)
{
    mountpoint->user_data = user_data;
    mountpoint->dispose = dispose;
}
