#include "webfuse/provider/impl/operation/read.h"

#include <stdlib.h>

#include "webfuse/provider/impl/operation/error.h"
#include "webfuse/provider/impl/request.h"
#include "webfuse/core/util.h"
#include "webfuse/core/base64.h"

void wfp_impl_read(
    struct wfp_impl_invokation_context * context,
    json_t * params,
    int id)
{
    size_t const count = json_array_size(params);
    if (5 == count)
    {
        json_t * inode_holder = json_array_get(params, 1);
        json_t * handle_holder = json_array_get(params, 2);
        json_t * offset_holder = json_array_get(params, 3);
        json_t * length_holder = json_array_get(params, 4);

        if (json_is_integer(inode_holder) &&
            json_is_integer(handle_holder) &&
            json_is_integer(offset_holder) &&
            json_is_integer(length_holder))
        {
            ino_t inode = (ino_t) json_integer_value(inode_holder);
            int handle = json_integer_value(handle_holder);
            size_t offset = json_integer_value(offset_holder);
            size_t length = json_integer_value(length_holder);
            struct wfp_request * request = wfp_impl_request_create(context->request, id);

            context->provider->read(request, inode, handle, offset, length, context->user_data);
        }
    } 
}

void wfp_impl_read_default(
    struct wfp_request * request,
    ino_t WF_UNUSED_PARAM(inode),
    uint32_t WF_UNUSED_PARAM(handle),
    size_t WF_UNUSED_PARAM(offset),
    size_t WF_UNUSED_PARAM(length),
    void * WF_UNUSED_PARAM(user_data))
{
    wfp_impl_respond_error(request, WF_BAD_NOENTRY);
}

void wfp_impl_respond_read(
    struct wfp_request * request,
    char const * data,
    size_t length)
{
    if (0 < length)
    {
        size_t const size = wf_base64_encoded_size(length) + 1;
        char * buffer = malloc(size);
        wf_base64_encode((uint8_t const *) data, length, buffer, size);

        json_t * result = json_object();
        json_object_set_new(result, "data", json_string(buffer));
        json_object_set_new(result, "format", json_string("base64"));
        json_object_set_new(result, "count", json_integer((int) length));

        wfp_impl_respond(request, result);
        free(buffer);
    }
    else
    {
            json_t * result = json_object();
            json_object_set_new(result, "data", json_string(""));
            json_object_set_new(result, "format", json_string("identity"));
            json_object_set_new(result, "count", json_integer(0));

            wfp_impl_respond(request, result);        
    }
}