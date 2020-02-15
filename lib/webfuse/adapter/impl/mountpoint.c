#include "webfuse/adapter/impl/mountpoint.h"

#include <stdlib.h>
#include <string.h>

struct wf_mountpoint
{
    char * path;
    wf_mountpoint_ondispose_fn * ondispose;
};

static void wf_impl_mountpoint_default_ondispose(
    struct wf_mountpoint * mountpoint)
{
    (void) mountpoint;
    // empty
}

struct wf_mountpoint *
wf_impl_mountpoint_create(
    char const * path)
{
    struct wf_mountpoint * mountpoint = malloc(sizeof(struct wf_mountpoint));
    mountpoint->path = strdup(path);
    mountpoint->ondispose = &wf_impl_mountpoint_default_ondispose;

    return mountpoint;
}

void 
wf_impl_mountpoint_dispose(
    struct wf_mountpoint * mountpoint)
{
    mountpoint->ondispose(mountpoint);

    free(mountpoint->path);
    free(mountpoint);
}

char const * 
wf_impl_mountpoint_get_path(
    struct wf_mountpoint const * mountpoint)
{
    return mountpoint->path;
}

void
wf_impl_mountpoint_set_ondispose(
    struct wf_mountpoint * mountpoint,
    wf_mountpoint_ondispose_fn * ondispose)
{
    mountpoint->ondispose = ondispose;
}
