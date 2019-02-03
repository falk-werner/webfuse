#include "wsfs/operations.h"

#include <errno.h>
#include <jansson.h>

#include "wsfs/jsonrpc.h"
#include "wsfs/util.h"

void wsfs_operation_open(
	fuse_req_t request,
	fuse_ino_t inode,
	struct fuse_file_info * file_info)
{
    struct wsfs_operations_context * user_data = fuse_req_userdata(request);
    struct wsfs_jsonrpc * rpc = user_data->rpc;

	json_t * result = NULL;
	wsfs_status status = wsfs_jsonrpc_invoke(rpc, &result, "open", "ii", inode, file_info->flags);
	if (NULL != result)
	{
        json_t * handle_holder = json_object_get(result, "handle");
        if ((NULL != handle_holder) && (json_is_integer(handle_holder))) 
        {
            file_info->fh = json_integer_value(handle_holder);
        }
        else
        {
            status = WSFS_BAD_FORMAT;
        }        

		json_decref(result);
	}

	if (WSFS_GOOD == status)
	{
		fuse_reply_open(request, file_info);		
	}
	else
	{
		fuse_reply_err(request, ENOENT);
	}
}
