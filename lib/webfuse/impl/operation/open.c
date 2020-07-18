#include "webfuse/impl/operation/open.h"
#include "webfuse/impl/operation/context.h"

#include "webfuse/impl/jsonrpc/proxy.h"
#include "webfuse/impl/json/node.h"
#include "webfuse/impl/util/util.h"
#include "webfuse/status.h"
#include "webfuse/impl/util/json_util.h"

#include <string.h>
#include <errno.h>

void wf_impl_operation_open_finished(
	void * user_data,
	struct wf_json const * result,
	struct wf_jsonrpc_error const * error)
{
	wf_status status = wf_impl_jsonrpc_get_status(error);
	fuse_req_t request = user_data;
	struct fuse_file_info file_info;
	memset(&file_info, 0, sizeof(struct fuse_file_info));

	if (NULL != result)
	{
        struct wf_json const * handle_holder = wf_impl_json_object_get(result, "handle");
        if (wf_impl_json_is_int(handle_holder))
        {
            file_info.fh = wf_impl_json_int_get(handle_holder);
        }
        else
        {
            status = WF_BAD_FORMAT;
        }        
	}

	if (WF_GOOD == status)
	{
		fuse_reply_open(request, &file_info);		
	}
	else
	{
		fuse_reply_err(request, ENOENT);
	}

}

void wf_impl_operation_open(
	fuse_req_t request,
	fuse_ino_t inode,
	struct fuse_file_info * file_info)
{
    struct wf_impl_operation_context * user_data = fuse_req_userdata(request);
    struct wf_jsonrpc_proxy * rpc = wf_impl_operation_context_get_proxy(user_data);

	if (NULL != rpc)
	{
		wf_impl_jsonrpc_proxy_invoke(rpc, &wf_impl_operation_open_finished, request, "open", "sii", user_data->name, inode, file_info->flags);
	}
	else
	{
		fuse_reply_err(request, ENOENT);
	}	
}
