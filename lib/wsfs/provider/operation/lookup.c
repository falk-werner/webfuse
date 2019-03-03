#include "wsfs/provider/operation/lookup_intern.h"

#include <stdbool.h>

#include "wsfs/provider/operation/error.h"
#include "wsfs/util.h"

void wsfsp_lookup(
    struct wsfsp_invokation_context * context,
    json_t * params,
    int id)
{
    size_t const count = json_array_size(params);
    if (2 == count)
    {
        json_t * inode_holder = json_array_get(params, 0);
        json_t * name_holder = json_array_get(params, 1);

        if (json_is_integer(inode_holder) &&
            json_is_string(name_holder))
        {
            ino_t inode = json_integer_value(inode_holder);
            char const * name = json_string_value(name_holder);

            struct wsfsp_request * request = wsfsp_request_create(context->request, id);
            context->provider->lookup(request, inode, name, context->user_data);
        }
    }
}

void wsfsp_respond_lookup(
    struct wsfsp_request * request,
    struct stat const * stat)
{
    bool const is_file = (0 != (stat->st_mode & S_IFREG));
    bool const is_dir = (0 != (stat->st_mode & S_IFDIR));

    json_t * result = json_object();
    json_object_set_new(result, "inode", json_integer(stat->st_ino));
    json_object_set_new(result, "mode", json_integer(stat->st_mode & 0777));    
    json_object_set_new(result, "atime", json_integer(stat->st_atime));
    json_object_set_new(result, "mtime", json_integer(stat->st_mtime));
    json_object_set_new(result, "ctime", json_integer(stat->st_ctime));

    if (is_file)
    {
        json_object_set_new(result, "type", json_string("file"));
        json_object_set_new(result, "size", json_integer(stat->st_size));
    }

    if (is_dir)
    {
        json_object_set_new(result, "type", json_string("dir"));
    }

    wsfsp_respond(request, result);    
}

void wsfsp_lookup_default(
    struct wsfsp_request * request,
    ino_t WSFS_UNUSED_PARAM(parent),
    char const * WSFS_UNUSED_PARAM(name),
    void * WSFS_UNUSED_PARAM(user_data))
{
    wsfsp_respond_error(request, -1);
}

