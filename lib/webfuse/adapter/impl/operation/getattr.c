#include "webfuse/adapter/impl/operation/getattr.h"
#include "webfuse/adapter/impl/operation/context.h"

#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> 

#include "webfuse/core/jsonrpc/proxy.h"
#include "webfuse/core/json_util.h"
#include "webfuse/core/util.h"

void wf_impl_operation_getattr_finished(
	void * user_data,
	json_t const * result,
	json_t const * error)
{
	wf_status status = wf_impl_jsonrpc_get_status(error);
	struct wf_impl_operation_getattr_context * context = user_data;

    struct stat buffer;
	if (NULL != result)
	{
		json_t * mode_holder = json_object_get(result, "mode");
		json_t * type_holder = json_object_get(result, "type");
		if ((json_is_integer(mode_holder)) && (json_is_string(type_holder)))
		{
            memset(&buffer, 0, sizeof(struct stat));

			buffer.st_ino = context->inode;
			buffer.st_mode = json_integer_value(mode_holder) & 0555;
			char const * type = json_string_value(type_holder);
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

		wf_jsonrpc_proxy_invoke(rpc, &wf_impl_operation_getattr_finished, getattr_context, "getattr", "si", user_data->name, inode);
	}
	else
	{
		fuse_reply_err(request, ENOENT);
	}
}
