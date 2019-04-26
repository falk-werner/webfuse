#include "webfuse/provider/impl/operation/open.h"
#include "webfuse/provider/impl/operation/error.h"
#include "webfuse/provider/impl/request.h"
#include "webfuse/core/util.h"

void wfp_impl_open(
    struct wfp_impl_invokation_context * context,
    json_t * params,
    int id)
{
    size_t const count = json_array_size(params);
    if (3 == count)
    {
        json_t * inode_holder = json_array_get(params, 1);
        json_t * flags_holder = json_array_get(params, 2);

        if (json_is_integer(inode_holder) &&
            json_is_integer(flags_holder))
        {
            ino_t inode = (ino_t) json_integer_value(inode_holder);
            int flags = (ino_t) json_integer_value(flags_holder);

            struct wfp_request * request = wfp_impl_request_create(context->request, id);

            context->provider->open(request, inode, flags, context->user_data);
        }
    }
}

void wfp_impl_open_default(
    struct wfp_request * request,
    ino_t WF_UNUSED_PARAM(inode),
    int WF_UNUSED_PARAM(flags),
    void * WF_UNUSED_PARAM(user_data))
{
    wfp_impl_respond_error(request, WF_BAD_NOENTRY);
}

void wfp_impl_respond_open(
    struct wfp_request * request,
    uint32_t handle)
{
    json_t * result = json_object();
    json_object_set_new(result, "handle", json_integer((int) handle));

    wfp_impl_respond(request, result);
}
