#include "webfuse/provider/impl/dirbuffer.h"
#include <stdlib.h>

struct wfp_dirbuffer * wfp_impl_dirbuffer_create(void)
{
    struct wfp_dirbuffer * buffer = malloc(sizeof(struct wfp_dirbuffer));
    buffer->entries = json_array();

    return buffer;
}

void wfp_impl_dirbuffer_dispose(
    struct wfp_dirbuffer * buffer)
{
    if (NULL != buffer->entries)
    {
        json_decref(buffer->entries);
    }

    free(buffer);
}

void wfp_impl_dirbuffer_add(
    struct wfp_dirbuffer * buffer,
    char const * name,
    ino_t inode)
{
    json_t * entry = json_object();
    json_object_set_new(entry, "name", json_string(name));
    json_object_set_new(entry, "inode", json_integer(inode));

    json_array_append_new(buffer->entries, entry);
}

json_t * wfp_impl_dirbuffer_take(
    struct wfp_dirbuffer * buffer)
{
    json_t * entries = buffer->entries;

    buffer->entries = NULL;
    return entries;
}
