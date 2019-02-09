#include "wsfs/operations.h"

#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> 

#include "wsfs/jsonrpc/server.h"
#include "wsfs/jsonrpc/util.h"
#include "wsfs/util.h"

struct wsfs_operation_getattr_context
{
	fuse_req_t request;
	double timeout;
	uid_t uid;
	gid_t gid;
};

static void wsfs_operation_getattr_finished(
	void * user_data,
	wsfs_status status,
	json_t const * data)
{
	struct wsfs_operation_getattr_context * context = user_data;

    struct stat buffer;
	if (NULL != data)
	{
		json_t * mode_holder = json_object_get(data, "mode");
		json_t * type_holder = json_object_get(data, "type");
		if ((NULL != mode_holder) && (json_is_integer(mode_holder)) && 
		    (NULL != type_holder) && (json_is_string(type_holder)))
		{
            memset(&buffer, 0, sizeof(struct stat));

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
			buffer.st_size = wsfs_json_get_int(data, "size", 0);
			buffer.st_atime = wsfs_json_get_int(data, "atime", 0);
			buffer.st_mtime = wsfs_json_get_int(data, "mtime", 0);
			buffer.st_ctime = wsfs_json_get_int(data, "ctime", 0);

		}
		else
		{
	        status = WSFS_BAD_FORMAT;
		}
	}

    if (WSFS_GOOD == status)
    {
        fuse_reply_attr(context->request, &buffer, context->timeout);
    }
    else
    {
	    fuse_reply_err(context->request, ENOENT);
    }

	free(context);
}

void wsfs_operation_getattr (
	fuse_req_t request,
	fuse_ino_t inode,
	struct fuse_file_info * WSFS_UNUSED_PARAM(file_info))
{
    struct fuse_ctx const * context = fuse_req_ctx(request);
    struct wsfs_operations_context * user_data = fuse_req_userdata(request);
    struct wsfs_jsonrpc_server * rpc = user_data->rpc;

	struct wsfs_operation_getattr_context * getattr_context = malloc(sizeof(struct wsfs_operation_getattr_context));
	getattr_context->request = request;
	getattr_context->uid = context->uid;
	getattr_context->gid = context->gid;
	getattr_context->timeout = user_data->timeout;

	wsfs_jsonrpc_server_invoke(rpc, &wsfs_operation_getattr_finished, getattr_context, "getattr", "i", inode);
}
