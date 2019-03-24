#include "wsfs/adapter/impl/operations.h"

#include <limits.h>
#include <errno.h>
#include <jansson.h>

#include "wsfs/adapter/impl/jsonrpc/server.h"
#include "wsfs/core/util.h"

void wsfs_impl_operation_close(
	fuse_req_t request,
	fuse_ino_t inode,
	struct fuse_file_info * file_info)
{
    struct wsfs_impl_operations_context * user_data = fuse_req_userdata(request);
    struct wsfs_impl_jsonrpc_server * rpc = user_data->rpc;

	int handle = (int) (file_info->fh & INT_MAX);
	wsfs_impl_jsonrpc_server_notify(rpc, "close", "iii", inode, handle, file_info->flags);
	fuse_reply_err(request, 0);
}
