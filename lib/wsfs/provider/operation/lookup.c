#include "wsfs/provider/operation/lookup_intern.h"
#include <stdio.h>
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
    (void) request;
    (void) stat;

    wsfsp_respond_error(request, -1);
}

void wsfsp_lookup_default(
    struct wsfsp_request * request,
    ino_t WSFS_UNUSED_PARAM(parent),
    char const * WSFS_UNUSED_PARAM(name),
    void * WSFS_UNUSED_PARAM(user_data))
{
    wsfsp_respond_error(request, -1);
}

