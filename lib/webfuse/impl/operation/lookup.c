#include "webfuse/impl/operation/lookup.h"
#include "webfuse/impl/operation/context.h"

#include <limits.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> 

#include <stdlib.h>

#include "webfuse/impl/jsonrpc/proxy.h"
#include "webfuse/impl/json/node.h"
#include "webfuse/impl/util/json_util.h"
#include "webfuse/impl/util/util.h"

void wf_impl_operation_lookup_finished(
	void * user_data,
	struct wf_json const * result,
	struct wf_jsonrpc_error const * error
)
{
	wf_status status = wf_impl_jsonrpc_get_status(error);
	struct wf_impl_operation_lookup_context * context = user_data; 	
    struct fuse_entry_param buffer;

	if (NULL != result)
	{
		struct wf_json const * inode_holder = wf_impl_json_object_get(result, "inode"); 
		struct wf_json const * mode_holder = wf_impl_json_object_get(result, "mode");
		struct wf_json const * type_holder = wf_impl_json_object_get(result, "type");
		if ((wf_impl_json_is_int(inode_holder)) &&
			(wf_impl_json_is_int(mode_holder)) && 
		    (wf_impl_json_is_string(type_holder)))
		{
            memset(&buffer, 0, sizeof(struct stat));

			buffer.ino = wf_impl_json_int_get(inode_holder);
			buffer.attr.st_ino = buffer.ino;
			buffer.attr.st_mode = wf_impl_json_int_get(mode_holder) & 0555;
			char const * type = wf_impl_json_string_get(type_holder);
			if (0 == strcmp("file", type)) 
			{
				buffer.attr.st_mode |= S_IFREG;
			}
			else if (0 == strcmp("dir", type))
			{
				buffer.attr.st_mode |= S_IFDIR;
			}


			buffer.attr_timeout = context->timeout;
			buffer.entry_timeout = context->timeout;
            buffer.attr.st_uid = context->uid;
            buffer.attr.st_gid = context->gid;
            buffer.attr.st_nlink = 1;
			buffer.attr.st_size = wf_impl_json_get_int(result, "size", 0);
			buffer.attr.st_atime = wf_impl_json_get_int(result, "atime", 0);
			buffer.attr.st_mtime = wf_impl_json_get_int(result, "mtime", 0);
			buffer.attr.st_ctime = wf_impl_json_get_int(result, "ctime", 0);
		}
		else
		{
	        status = WF_BAD_FORMAT;
		}
	}

    if (WF_GOOD == status)
    {
        fuse_reply_entry(context->request, &buffer);
    }
    else
    {
	    fuse_reply_err(context->request, ENOENT);
    }

	free(context);
}

void wf_impl_operation_lookup (
	fuse_req_t request, 
	fuse_ino_t parent, 
	char const * name)
{
    struct fuse_ctx const * context = fuse_req_ctx(request);
    struct wf_impl_operation_context * user_data = fuse_req_userdata(request);
    struct wf_jsonrpc_proxy * rpc = wf_impl_operation_context_get_proxy(user_data);

	if (NULL != rpc)
	{
		struct wf_impl_operation_lookup_context * lookup_context = malloc(sizeof(struct wf_impl_operation_lookup_context));
		lookup_context->request = request;
		lookup_context->uid = context->uid;
		lookup_context->gid = context->gid;
		lookup_context->timeout = user_data->timeout;

		wf_impl_jsonrpc_proxy_invoke(rpc, &wf_impl_operation_lookup_finished, lookup_context, "lookup", "sis", user_data->name, (int) (parent & INT_MAX), name);
	}
	else
	{
		fuse_reply_err(request, ENOENT);
	}	
}
