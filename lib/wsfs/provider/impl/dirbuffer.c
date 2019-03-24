#include "wsfs/provider/impl/dirbuffer.h"
#include <stdlib.h>

struct wsfsp_dirbuffer * wsfsp_impl_dirbuffer_create(void)
{
    struct wsfsp_dirbuffer * buffer = malloc(sizeof(struct wsfsp_dirbuffer));
    if (NULL != buffer)
    {
        buffer->entries = json_array();
    }

    return buffer;
}

void wsfsp_impl_dirbuffer_dispose(
    struct wsfsp_dirbuffer * buffer)
{
    if (NULL != buffer->entries)
    {
        json_decref(buffer->entries);
    }

    free(buffer);
}

void wsfsp_impl_dirbuffer_add(
    struct wsfsp_dirbuffer * buffer,
    char const * name,
    ino_t inode)
{
    json_t * entry = json_object();
    json_object_set_new(entry, "name", json_string(name));
    json_object_set_new(entry, "inode", json_integer(inode));

    json_array_append_new(buffer->entries, entry);
}

json_t * wsfsp_impl_dirbuffer_take(
    struct wsfsp_dirbuffer * buffer)
{
    json_t * entries = buffer->entries;

    buffer->entries = NULL;
    return entries;
}
