#include "webfuse/provider/impl/operation/getattr.h"

#include <stdbool.h>

#include "webfuse/provider/impl/operation/error.h"
#include "webfuse/provider/impl/request.h"
#include "webfuse/core/util.h"


void wfp_impl_getattr(
    struct wfp_impl_invokation_context * context,
    json_t * params,
    int id)
{
    size_t const count = json_array_size(params);
    if (2 == count)
    {
        json_t * inode_holder = json_array_get(params, 1);

        if (json_is_integer(inode_holder))
        {
            ino_t inode = (ino_t) json_integer_value(inode_holder);
            struct wfp_request * request = wfp_impl_request_create(context->request, id);

            context->provider->getattr(request, inode, context->user_data);
        }
    }
}

void wfp_impl_getattr_default(
    struct wfp_request * request,
    ino_t WF_UNUSED_PARAM(inode),
    void * WF_UNUSED_PARAM(user_data))
{
    wfp_impl_respond_error(request, WF_BAD_NOENTRY);
}

void wfp_impl_respond_getattr(
    struct wfp_request * request,
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

    wfp_impl_respond(request, result);
}
