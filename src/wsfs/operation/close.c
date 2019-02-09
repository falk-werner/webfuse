#include "wsfs/operations.h"

#include <limits.h>
#include <errno.h>
#include <jansson.h>

#include "wsfs/jsonrpc/server.h"
#include "wsfs/util.h"


static void wsfs_operation_close_finished(
	void * user_data,
	wsfs_status status,
	json_t const * WSFS_UNUSED_PARAM(result))
{
	fuse_req_t request = (fuse_req_t) user_data;
	fuse_reply_err(request, (WSFS_GOOD == status) ? 0 : ENOENT);
}

void wsfs_operation_close(
	fuse_req_t request,
	fuse_ino_t inode,
	struct fuse_file_info * file_info)
{
    struct wsfs_operations_context * user_data = fuse_req_userdata(request);
    struct wsfs_jsonrpc_server * rpc = user_data->rpc;

	int handle = (int) (file_info->fh & INT_MAX);
	wsfs_jsonrpc_server_invoke(
		rpc, &wsfs_operation_close_finished, request,
		"close", "iii", inode, handle, file_info->flags);
}
