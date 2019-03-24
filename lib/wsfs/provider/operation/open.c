#include "wsfs/provider/operation/open_intern.h"
#include "wsfs/provider/operation/error.h"
#include "wsfs/provider/request.h"
#include "wsfs/core/util.h"

void wsfsp_open(
    struct wsfsp_invokation_context * context,
    json_t * params,
    int id)
{
    size_t const count = json_array_size(params);
    if (2 == count)
    {
        json_t * inode_holder = json_array_get(params, 0);
        json_t * flags_holder = json_array_get(params, 1);

        if (json_is_integer(inode_holder) &&
            json_is_integer(flags_holder))
        {
            ino_t inode = (ino_t) json_integer_value(inode_holder);
            int flags = (ino_t) json_integer_value(flags_holder);

            struct wsfsp_request * request = wsfsp_request_create(context->request, id);

            context->provider->open(request, inode, flags, context->user_data); /* Flawfinder: ignore */
        }
    }
}

void wsfsp_open_default(
    struct wsfsp_request * request,
    ino_t WSFS_UNUSED_PARAM(inode),
    int WSFS_UNUSED_PARAM(flags),
    void * WSFS_UNUSED_PARAM(user_data))
{
    wsfsp_respond_error(request, WSFS_BAD_NOENTRY);
}

void wsfsp_respond_open(
    struct wsfsp_request * request,
    uint32_t handle)
{
    json_t * result = json_object();
    json_object_set_new(result, "handle", json_integer((int) handle));

    wsfsp_respond(request, result);
}
