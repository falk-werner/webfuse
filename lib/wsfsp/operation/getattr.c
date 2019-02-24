#include "wsfsp/operation/getattr_intern.h"
#include <stdio.h>

#include "wsfsp/operation/error.h"


void wsfsp_getattr(
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

            context->provider->getattr(request, inode, context->user_data);
        }
    }
}

void wsfsp_getattr_default(
    struct wsfsp_request * request,
    ino_t inode,
    void * user_data)
{
    (void) inode;
    (void) user_data;

    wsfsp_respond_error(request, -1);
}

void wsfsp_respond_getattr(
    struct wsfsp_request * request,
    struct stat const * stat)
{
    (void) request;
    (void) stat;

    // ToDo: implement me
}
