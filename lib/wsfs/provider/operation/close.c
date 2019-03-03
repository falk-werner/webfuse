#include "wsfs/provider/operation/close_intern.h"
#include <limits.h>
#include "wsfs/util.h"

void wsfsp_close(
    struct wsfsp_invokation_context * context,
    json_t * params,
    int WSFS_UNUSED_PARAM(id))
{
    size_t const param_count = json_array_size(params);
    if (3 == param_count)
    {
        json_t * inode_holder = json_array_get(params, 0);
        json_t * handle_holder = json_array_get(params, 1);
        json_t * flags_holder = json_array_get(params, 2);

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

void wsfsp_close_default(
    ino_t WSFS_UNUSED_PARAM(inode),
    uint32_t WSFS_UNUSED_PARAM(handle),
    int WSFS_UNUSED_PARAM(flags),
    void * WSFS_UNUSED_PARAM(user_data))
{
    // empty
}