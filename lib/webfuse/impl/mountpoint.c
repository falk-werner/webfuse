#include "webfuse/impl/mountpoint.h"

#include <stdlib.h>
#include <string.h>

#define WF_MOUNTOPTIONS_INITIAL_CAPACITY 8

struct wf_mountpoint *
wf_impl_mountpoint_create(
    char const * path)
{
    struct wf_mountpoint * mountpoint = malloc(sizeof(struct wf_mountpoint));
    mountpoint->path = strdup(path);
    mountpoint->user_data = NULL;
    mountpoint->dispose = NULL;
    mountpoint->options.size = 1;
    mountpoint->options.capacity = WF_MOUNTOPTIONS_INITIAL_CAPACITY;
    mountpoint->options.items = malloc(sizeof(char*) * mountpoint->options.capacity);
    mountpoint->options.items[0] = strdup("");
    mountpoint->options.items[1] = NULL;

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

    for(size_t i = 0; i < mountpoint->options.size; i++)
    {
        free(mountpoint->options.items[i]);
    }
    free(mountpoint->options.items);

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

void
wf_impl_mountpoint_add_mountoption(
    struct wf_mountpoint * mountpoint,
    char const * option)
{
    if ((mountpoint->options.size + 1) >= mountpoint->options.capacity)
    {
        mountpoint->options.capacity *= 2;
        mountpoint->options.items = realloc(mountpoint->options.items,
            sizeof(char*) * mountpoint->options.capacity);
    }

    mountpoint->options.items[mountpoint->options.size] = strdup(option);
    mountpoint->options.items[mountpoint->options.size + 1] = NULL;
    mountpoint->options.size++;
}
