#include "webfuse/adapter/impl/operation/open.h"
#include "webfuse/adapter/impl/operation/context.h"

#include "webfuse/core/jsonrpc/proxy.h"
#include "webfuse/core/util.h"
#include "webfuse/core/status.h"
#include "webfuse/core/json_util.h"

#include <string.h>
#include <errno.h>

void wf_impl_operation_open_finished(
	void * user_data,
	json_t const * result,
	json_t const * error)
{
	wf_status status = wf_impl_jsonrpc_get_status(error);
	fuse_req_t request = user_data;
	struct fuse_file_info file_info;
	memset(&file_info, 0, sizeof(struct fuse_file_info));

	if (NULL != result)
	{
        json_t * handle_holder = json_object_get(result, "handle");
        if (json_is_integer(handle_holder))
        {
            file_info.fh = json_integer_value(handle_holder);
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
		wf_jsonrpc_proxy_invoke(rpc, &wf_impl_operation_open_finished, request, "open", "sii", user_data->name, inode, file_info->flags);
	}
	else
	{
		fuse_reply_err(request, ENOENT);
	}	
}
