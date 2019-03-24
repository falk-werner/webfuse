#include "wsfs/provider/operation/readdir_intern.h"
#include "wsfs/provider/operation/error.h"
#include "wsfs/provider/dirbuffer_intern.h"
#include "wsfs/provider/request.h"
#include "wsfs/core/util.h"

void wsfsp_readdir(
    struct wsfsp_invokation_context * context,
    json_t * params,
    int id)
{
    size_t const count = json_array_size(params);
    if (1 == count)
    {
        json_t * inode_holder = json_array_get(params, 0);

        if ((NULL != inode_holder) && (json_is_integer(inode_holder)))
        {
            ino_t inode = (ino_t) json_integer_value(inode_holder);
            struct wsfsp_request * request = wsfsp_request_create(context->request, id);

            context->provider->readdir(request, inode, context->user_data);
        }
    }
}

void wsfsp_readdir_default(
    struct wsfsp_request * request,
    ino_t WSFS_UNUSED_PARAM(directory),
    void * WSFS_UNUSED_PARAM(user_data))
{
    wsfsp_respond_error(request, WSFS_BAD_NOENTRY);
}

void wsfsp_respond_readdir(
    struct wsfsp_request * request,
    struct wsfsp_dirbuffer * dirbuffer)
{
    json_t * result = wsfsp_dirbuffer_take(dirbuffer);
    wsfsp_respond(request, result);
}

