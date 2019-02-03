#include "wsfs/operations.h"

#include <limits.h>
#include <errno.h>
#include <jansson.h>

#include "wsfs/jsonrpc.h"
#include "wsfs/util.h"


void wsfs_operation_ll_close(
	fuse_req_t request,
	fuse_ino_t inode,
	struct fuse_file_info * file_info)
{
    struct wsfs_operations_context * user_data = fuse_req_userdata(request);
    struct wsfs_jsonrpc * rpc = user_data->rpc;

	json_t * result = NULL;
	int handle = (int) (file_info->fh & INT_MAX);
	wsfs_status const status = wsfs_jsonrpc_invoke(rpc, &result, "close", "iii", inode, handle, file_info->flags);
	if (NULL != result)
	{
		// unused
		json_decref(result);
	}


	fuse_reply_err(request, (WSFS_GOOD == status) ? 0 : ENOENT);
}
