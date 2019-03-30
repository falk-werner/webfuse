#include "webfuse/adapter/impl/operations.h"

#include <limits.h>
#include <errno.h>
#include <jansson.h>

#include "webfuse/adapter/impl/jsonrpc/proxy.h"
#include "webfuse/core/util.h"

void wf_impl_operation_close(
	fuse_req_t request,
	fuse_ino_t inode,
	struct fuse_file_info * file_info)
{
    struct wf_impl_operations_context * user_data = fuse_req_userdata(request);
    struct wf_impl_jsonrpc_proxy * rpc = wf_impl_operations_context_get_proxy(user_data, inode);

	if (NULL != rpc)
	{
		int handle = (int) (file_info->fh & INT_MAX);
		wf_impl_jsonrpc_proxy_notify(rpc, "close", "iii", inode, handle, file_info->flags);
	}
	
	fuse_reply_err(request, 0);
}
