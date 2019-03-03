#include "wsfs/provider/operation/read_intern.h"

#include <stdlib.h>
#include <libwebsockets.h>

#include "wsfs/provider/operation/error.h"
#include "wsfs/util.h"

void wsfsp_read(
    struct wsfsp_invokation_context * context,
    json_t * params,
    int id)
{
    size_t const count = json_array_size(params);
    if (4 == count)
    {
        json_t * inode_holder = json_array_get(params, 0);
        json_t * handle_holder = json_array_get(params, 1);
        json_t * offset_holder = json_array_get(params, 2);
        json_t * length_holder = json_array_get(params, 3);

        if (json_is_integer(inode_holder) &&
            json_is_integer(handle_holder) &&
            json_is_integer(offset_holder) &&
            json_is_integer(length_holder))
        {
            ino_t inode = (ino_t) json_integer_value(inode_holder);
            int handle = json_integer_value(handle_holder);
            size_t offset = json_integer_value(offset_holder);
            size_t length = json_integer_value(length_holder);
            struct wsfsp_request * request = wsfsp_request_create(context->request, id);

            context->provider->read(request, inode, handle, offset, length, context->user_data);
        }
    } 
}

void wsfsp_read_default(
    struct wsfsp_request * request,
    ino_t WSFS_UNUSED_PARAM(inode),
    uint32_t WSFS_UNUSED_PARAM(handle),
    size_t WSFS_UNUSED_PARAM(offset),
    size_t WSFS_UNUSED_PARAM(length),
    void * WSFS_UNUSED_PARAM(user_data))
{
    wsfsp_respond_error(request, -1);
}

void wsfsp_respond_read(
    struct wsfsp_request * request,
    char const * data,
    size_t length)
{
    if (0 < length)
    {
        size_t const size = 4 * ((length / 3) + 2);
        char * buffer = malloc(size);
        if (NULL != buffer)
        {
            lws_b64_encode_string(data, length, buffer, size);

            json_t * result = json_object();
            json_object_set_new(result, "data", json_string(buffer));
            json_object_set_new(result, "format", json_string("base64"));
            json_object_set_new(result, "count", json_integer((int) length));

            wsfsp_respond(request, result);
            free(buffer);
        }
        else
        {
            wsfsp_respond_error(request, -1);
        }
    }
    else
    {
            json_t * result = json_object();
            json_object_set_new(result, "data", json_string(""));
            json_object_set_new(result, "format", json_string("identitiy"));
            json_object_set_new(result, "count", json_integer(0));

            wsfsp_respond(request, result);        
    }
}