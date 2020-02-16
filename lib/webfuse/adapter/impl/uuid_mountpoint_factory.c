#include "webfuse/adapter/impl/uuid_mountpoint_factory.h"
#include "webfuse/adapter/impl/uuid_mountpoint.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>

struct wf_impl_uuid_mountpoint_factory
{
    char * root_path;
};

void *
wf_impl_uuid_mountpoint_factory_create(
    char * root_path)
{
	mkdir(root_path, 0755);

    struct wf_impl_uuid_mountpoint_factory * factory = malloc(sizeof(struct wf_impl_uuid_mountpoint_factory));
    factory->root_path = strdup(root_path);

    return factory;
}

void
wf_impl_uuid_mountpoint_factory_dispose(
    void * user_data)
{
    struct wf_impl_uuid_mountpoint_factory * factory = user_data;

 	rmdir(factory->root_path);
    free(factory->root_path);
    free(factory);
}

struct wf_mountpoint *
wf_impl_uuid_mountpoint_factory_create_mountpoint(
    char const * filesystem,
    void * user_data)
{
    struct wf_impl_uuid_mountpoint_factory * factory = user_data;

    return wf_impl_uuid_mountpoint_create(factory->root_path, filesystem);
}
