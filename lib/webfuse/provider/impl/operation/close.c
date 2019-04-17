#include "webfuse/provider/impl/operation/close.h"
#include <limits.h>
#include "webfuse/core/util.h"

void wfp_impl_close(
    struct wfp_impl_invokation_context * context,
    json_t * params,
    int WF_UNUSED_PARAM(id))
{
    size_t const param_count = json_array_size(params);
    if (4 == param_count)
    {
        json_t * inode_holder = json_array_get(params, 1);
        json_t * handle_holder = json_array_get(params, 2);
        json_t * flags_holder = json_array_get(params, 3);

        if (json_is_integer(inode_holder) && 
            json_is_integer(handle_holder) &&
            json_is_integer(flags_holder))
        {
            ino_t inode = (ino_t) json_integer_value(inode_holder);
            uint32_t handle = (uint32_t) (json_integer_value(handle_holder) & UINT32_MAX);
            int flags = json_integer_value(flags_holder);

            context->provider->close(inode, handle, flags, context->user_data);
        }
    }

}

void wfp_impl_close_default(
    ino_t WF_UNUSED_PARAM(inode),
    uint32_t WF_UNUSED_PARAM(handle),
    int WF_UNUSED_PARAM(flags),
    void * WF_UNUSED_PARAM(user_data))
{
    // empty
}