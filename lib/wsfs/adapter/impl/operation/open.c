#include "wsfs/adapter/impl/operations.h"

#include <string.h>
#include <errno.h>
#include <jansson.h>

#include "wsfs/adapter/impl/jsonrpc/server.h"
#include "wsfs/util.h"
#include "wsfs/status.h"

static void operation_open_finished(
	void * user_data,
	wsfs_status status,
	json_t const * result)
{
	fuse_req_t request = user_data;
	struct fuse_file_info file_info;
	memset(&file_info, 0, sizeof(struct fuse_file_info));

	if (NULL != result)
	{
        json_t * handle_holder = json_object_get(result, "handle");
        if ((NULL != handle_holder) && (json_is_integer(handle_holder))) 
        {
            file_info.fh = json_integer_value(handle_holder);
        }
        else
        {
            status = WSFS_BAD_FORMAT;
        }        
	}

	if (WSFS_GOOD == status)
	{
		fuse_reply_open(request, &file_info);		
	}
	else
	{
		fuse_reply_err(request, ENOENT);
	}

}

void operation_open(
	fuse_req_t request,
	fuse_ino_t inode,
	struct fuse_file_info * file_info)
{
    struct operations_context * user_data = fuse_req_userdata(request);
    struct jsonrpc_server * rpc = user_data->rpc;

	jsonrpc_server_invoke(rpc, &operation_open_finished, request, "open", "ii", inode, file_info->flags);
}
