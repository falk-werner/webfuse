#include "wsfs/operations.h"

#include <jansson.h>
#include "wsfs/jsonrpc.h"

int wsfs_operation_open(
	char const *path,
	struct fuse_file_info * file_info)
{
	struct fuse_context * context = fuse_get_context();
    struct wsfs_jsonrpc * rpc = context->private_data;

	json_t * result = NULL;
	wsfs_status status = wsfs_jsonrpc_invoke(rpc, &result, "open", "si", path, file_info->flags);
	if (NULL != result)
	{
        json_t * handle_holder = json_object_get(result, "handle");
        if ((NULL != handle_holder) && (json_is_integer(handle_holder))) 
        {
            file_info->fh = json_integer_value(handle_holder);
        }
        else
        {
            status = WSFS_BAD_FORMAT;
        }        

		json_decref(result);
	}

	return wsfs_status_to_rc(status);
}
