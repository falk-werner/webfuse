#include "wsfs/adapter/operations.h"

#include <limits.h>
#include <errno.h>
#include <jansson.h>

#include "wsfs/adapter/jsonrpc/server.h"
#include "wsfs/util.h"

void wsfs_operation_close(
	fuse_req_t request,
	fuse_ino_t inode,
	struct fuse_file_info * file_info)
{
    struct wsfs_operations_context * user_data = fuse_req_userdata(request);
    struct wsfs_jsonrpc_server * rpc = user_data->rpc;

	int handle = (int) (file_info->fh & INT_MAX);
	wsfs_jsonrpc_server_notify(rpc, "close", "iii", inode, handle, file_info->flags);
	fuse_reply_err(request, 0);
}
