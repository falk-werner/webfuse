#include "webfuse/adapter/impl/operation/close.h"
#include "webfuse/adapter/impl/operation/context.h"

#include <limits.h>
#include <errno.h>
#include <jansson.h>

#include "webfuse/core/jsonrpc/proxy.h"
#include "webfuse/core/util.h"

void wf_impl_operation_close(
	fuse_req_t request,
	fuse_ino_t inode,
	struct fuse_file_info * file_info)
{
    struct wf_impl_operation_context * user_data = fuse_req_userdata(request);
    struct wf_jsonrpc_proxy * rpc = wf_impl_operation_context_get_proxy(user_data);

	if (NULL != rpc)
	{
		int handle = (int) (file_info->fh & INT_MAX);
		wf_jsonrpc_proxy_notify(rpc, "close", "siii", user_data->name, inode, handle, file_info->flags);
	}
	
	fuse_reply_err(request, 0);
}
