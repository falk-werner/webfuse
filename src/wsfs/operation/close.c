#include "wsfs/operations.h"

#include <limits.h>

#include <jansson.h>
#include "wsfs/jsonrpc.h"

int wsfs_operation_close(
	char const *path,
	struct fuse_file_info * file_info)
{
	struct fuse_context * context = fuse_get_context();
    struct wsfs_jsonrpc * rpc = context->private_data;

	json_t * result = NULL;
	int handle = (int) (file_info->fh & INT_MAX);
	wsfs_status const status = wsfs_jsonrpc_invoke(rpc, &result, "close", "sii", path, handle, file_info->flags);
	if (NULL != result)
	{
		// unused
		json_decref(result);
	}

	return wsfs_status_to_rc(status);
}
