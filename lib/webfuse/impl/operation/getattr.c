#include "webfuse/impl/operation/getattr.h"
#include "webfuse/impl/operation/context.h"

#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> 

#include "webfuse/impl/jsonrpc/proxy.h"
#include "webfuse/impl/util/json_util.h"
#include "webfuse/impl/util/util.h"
#include "webfuse/impl/json/node.h"

void wf_impl_operation_getattr_finished(
	void * user_data,
	struct wf_json const * result,
	struct wf_jsonrpc_error const * error)
{
	wf_status status = wf_impl_jsonrpc_get_status(error);
	struct wf_impl_operation_getattr_context * context = user_data;

    struct stat buffer;
	if (NULL != result)
	{
		struct wf_json const * mode_holder = wf_impl_json_object_get(result, "mode");
		struct wf_json const * type_holder = wf_impl_json_object_get(result, "type");
		if ((wf_impl_json_is_int(mode_holder)) && (wf_impl_json_is_string(type_holder)))
		{
            memset(&buffer, 0, sizeof(struct stat));

			buffer.st_ino = context->inode;
			buffer.st_mode = wf_impl_json_int_get(mode_holder) & 0555;
			char const * type = wf_impl_json_string_get(type_holder);
			if (0 == strcmp("file", type)) 
			{
				buffer.st_mode |= S_IFREG;
			}
			else if (0 == strcmp("dir", type))
			{
				buffer.st_mode |= S_IFDIR;
			}

            buffer.st_uid = context->uid;
            buffer.st_gid = context->gid;
            buffer.st_nlink = 1;
			buffer.st_size = wf_impl_json_get_int(result, "size", 0);
			buffer.st_atime = wf_impl_json_get_int(result, "atime", 0);
			buffer.st_mtime = wf_impl_json_get_int(result, "mtime", 0);
			buffer.st_ctime = wf_impl_json_get_int(result, "ctime", 0);
		}
		else
		{
	        status = WF_BAD_FORMAT;
		}
	}

    if (WF_GOOD == status)
    {
        fuse_reply_attr(context->request, &buffer, context->timeout);
    }
    else
    {
	    fuse_reply_err(context->request, ENOENT);
    }

	free(context);
}

void wf_impl_operation_getattr (
	fuse_req_t request,
	fuse_ino_t inode,
	struct fuse_file_info * WF_UNUSED_PARAM(file_info))
{
    struct fuse_ctx const * context = fuse_req_ctx(request);
    struct wf_impl_operation_context * user_data = fuse_req_userdata(request);
    struct wf_jsonrpc_proxy * rpc = wf_impl_operation_context_get_proxy(user_data);

	if (NULL != rpc)
	{
		struct wf_impl_operation_getattr_context * getattr_context = malloc(sizeof(struct wf_impl_operation_getattr_context));
		getattr_context->request = request;
		getattr_context->inode = inode;		
		getattr_context->uid = context->uid;
		getattr_context->gid = context->gid;
		getattr_context->timeout = user_data->timeout;

		wf_impl_jsonrpc_proxy_invoke(rpc, &wf_impl_operation_getattr_finished, getattr_context, "getattr", "si", user_data->name, inode);
	}
	else
	{
		fuse_reply_err(request, ENOENT);
	}
}
