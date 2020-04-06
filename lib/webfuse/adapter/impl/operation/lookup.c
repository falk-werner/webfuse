#include "webfuse/adapter/impl/operation/lookup.h"
#include "webfuse/adapter/impl/operation/context.h"

#include <limits.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> 

#include <stdlib.h>

#include "webfuse/core/jsonrpc/proxy.h"
#include "webfuse/core/json_util.h"
#include "webfuse/core/util.h"

void wf_impl_operation_lookup_finished(
	void * user_data,
	json_t const * result,
	json_t const * error
)
{
	wf_status status = wf_impl_jsonrpc_get_status(error);
	struct wf_impl_operation_lookup_context * context = user_data; 	
    struct fuse_entry_param buffer;

	if (NULL != result)
	{
		json_t * inode_holder = json_object_get(result, "inode"); 
		json_t * mode_holder = json_object_get(result, "mode");
		json_t * type_holder = json_object_get(result, "type");
		if ((json_is_integer(inode_holder)) &&
			(json_is_integer(mode_holder)) && 
		    (json_is_string(type_holder)))
		{
            memset(&buffer, 0, sizeof(struct stat));

			buffer.ino = json_integer_value(inode_holder);
			buffer.attr.st_ino = buffer.ino;
			buffer.attr.st_mode = json_integer_value(mode_holder) & 0555;
			char const * type = json_string_value(type_holder);
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

		wf_jsonrpc_proxy_invoke(rpc, &wf_impl_operation_lookup_finished, lookup_context, "lookup", "sis", user_data->name, (int) (parent & INT_MAX), name);
	}
	else
	{
		fuse_reply_err(request, ENOENT);
	}	
}
