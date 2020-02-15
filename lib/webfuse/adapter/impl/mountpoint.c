#include "webfuse/adapter/impl/mountpoint.h"

#include <stdlib.h>
#include <string.h>

struct wf_mountpoint
{
    char * path;
};

struct wf_mountpoint *
wf_impl_mountpoint_create(
    char const * path)
{
    struct wf_mountpoint * mountpoint = malloc(sizeof(struct wf_mountpoint));
    mountpoint->path = strdup(path);

    return mountpoint;
}

void 
wf_impl_mountpoint_dispose(
    struct wf_mountpoint * mountpoint)
{
    free(mountpoint->path);
    free(mountpoint);
}

char const * 
wf_impl_mountpoint_get_path(
    struct wf_mountpoint const * mountpoint)
{
    return mountpoint->path;
}
