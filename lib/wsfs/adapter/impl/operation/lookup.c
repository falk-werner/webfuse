#include "wsfs/adapter/impl/operations.h"

#include <limits.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> 

#include <stdlib.h>

#include "wsfs/adapter/impl/jsonrpc/server.h"
#include "wsfs/adapter/impl/jsonrpc/util.h"
#include "wsfs/util.h"

struct wsfs_operation_lookup_context
{
	fuse_req_t request;
	double timeout;
	uid_t uid;
	gid_t gid;
};

static void wsfs_operation_lookup_finished(
	void * user_data,
	wsfs_status status,
	json_t const * data
)
{
	struct wsfs_operation_lookup_context * context = user_data; 	
    struct fuse_entry_param buffer;

	if (NULL != data)
	{
		json_t * inode_holder = json_object_get(data, "inode"); 
		json_t * mode_holder = json_object_get(data, "mode");
		json_t * type_holder = json_object_get(data, "type");
		if ((NULL != inode_holder) && (json_is_integer(inode_holder)) &&
			(NULL != mode_holder) && (json_is_integer(mode_holder)) && 
		    (NULL != type_holder) && (json_is_string(type_holder)))
		{
            memset(&buffer, 0, sizeof(struct stat));

			buffer.ino = json_integer_value(inode_holder);
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
			buffer.attr.st_size = wsfs_json_get_int(data, "size", 0);
			buffer.attr.st_atime = wsfs_json_get_int(data, "atime", 0);
			buffer.attr.st_mtime = wsfs_json_get_int(data, "mtime", 0);
			buffer.attr.st_ctime = wsfs_json_get_int(data, "ctime", 0);
		}
		else
		{
	        status = WSFS_BAD_FORMAT;
		}
	}

    if (WSFS_GOOD == status)
    {
        fuse_reply_entry(context->request, &buffer);
    }
    else
    {
	    fuse_reply_err(context->request, ENOENT);
    }

	free(context);
}

void wsfs_operation_lookup (
	fuse_req_t request, 
	fuse_ino_t parent, 
	char const * name)
{
    struct fuse_ctx const * context = fuse_req_ctx(request);
    struct wsfs_operations_context * user_data = fuse_req_userdata(request);
    struct wsfs_jsonrpc_server * rpc = user_data->rpc;

	struct wsfs_operation_lookup_context * lookup_context = malloc(sizeof(struct wsfs_operation_lookup_context));
	lookup_context->request = request;
	lookup_context->uid = context->uid;
	lookup_context->gid = context->gid;
	lookup_context->timeout = user_data->timeout;

	wsfs_jsonrpc_server_invoke(rpc, &wsfs_operation_lookup_finished, lookup_context, "lookup", "is", (int) (parent & INT_MAX), name);

}
