#include "wsfs/adapter/impl/operations.h"

#include <limits.h>
#include <errno.h>
#include <jansson.h>

#include "wsfs/adapter/impl/jsonrpc/server.h"
#include "wsfs/util.h"

void operation_close(
	fuse_req_t request,
	fuse_ino_t inode,
	struct fuse_file_info * file_info)
{
    struct operations_context * user_data = fuse_req_userdata(request);
    struct jsonrpc_server * rpc = user_data->rpc;

	int handle = (int) (file_info->fh & INT_MAX);
	jsonrpc_server_notify(rpc, "close", "iii", inode, handle, file_info->flags);
	fuse_reply_err(request, 0);
}
