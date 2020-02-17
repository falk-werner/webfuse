#include "webfuse/adapter/impl/uuid_mountpoint_factory.h"
#include "webfuse/adapter/impl/uuid_mountpoint.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct wf_impl_uuid_mountpoint_factory
{
    char * root_path;
    bool root_created;
};

static void *
wf_impl_uuid_mountpoint_factory_create_context(
    char const * root_path)
{
    struct wf_impl_uuid_mountpoint_factory * factory = NULL;
    bool root_created = false;

    struct stat info;
    int rc = stat(root_path, &info);
    if ((0 != rc) || (!S_ISDIR(info.st_mode)))
    {
        rc = mkdir(root_path, 0755);
        root_created = true;
    }

    if (0 == rc)
    {
        factory = malloc(sizeof(struct wf_impl_uuid_mountpoint_factory));
        factory->root_path = strdup(root_path);
        factory->root_created = root_created;
    }

    return factory;
}

static void
wf_impl_uuid_mountpoint_factory_dispose(
    void * user_data)
{
    struct wf_impl_uuid_mountpoint_factory * factory = user_data;

    if (factory->root_created)
    {
 	    rmdir(factory->root_path);
    }

    free(factory->root_path);
    free(factory);
}

static struct wf_mountpoint *
wf_impl_uuid_mountpoint_factory_create_mountpoint(
    char const * filesystem,
    void * user_data)
{
    struct wf_impl_uuid_mountpoint_factory * factory = user_data;

    return wf_impl_uuid_mountpoint_create(factory->root_path, filesystem);
}

bool
wf_impl_uuid_mountpoint_factory_init(
    struct wf_impl_mountpoint_factory * factory,
    char const * root_path)
{
    void * context = wf_impl_uuid_mountpoint_factory_create_context(root_path);
    bool const result = (NULL != context);

    if (result)
    {
        factory->create_mountpoint = &wf_impl_uuid_mountpoint_factory_create_mountpoint;
        factory->user_data = context;
        factory->dispose = &wf_impl_uuid_mountpoint_factory_dispose;
    }

    return result;
}
