#include "wsfs/operations.h"

#include <string.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <unistd.h>

#include <jansson.h>

#include "wsfs/util.h"
#include "wsfs/jsonrpc.h"

int wsfs_operation_getattr(
	char const * path,
	struct stat * buffer,
	struct fuse_file_info * WSFS_UNUSED_PARAM(file_info))
{
	struct fuse_context * const context = fuse_get_context();
	struct wsfs_jsonrpc * const rpc = context->private_data;

	json_t * data = NULL;
	wsfs_status status = wsfs_jsonrpc_invoke(rpc, &data, "getattr", "s", path);
	if (NULL != data)
	{
		json_t * mode_holder = json_object_get(data, "mode");
		json_t * type_holder = json_object_get(data, "type");
		if ((NULL != mode_holder) && (json_is_integer(mode_holder)) && 
		    (NULL != type_holder) && (json_is_string(type_holder)))
		{
			buffer->st_mode = json_integer_value(mode_holder) & 0555;
			char const * type = json_string_value(type_holder);
			if (0 == strcmp("file", type)) 
			{
				buffer->st_mode |= S_IFREG;
			}
			else if (0 == strcmp("dir", type))
			{
				buffer->st_mode |= S_IFDIR;
			}

            buffer->st_uid = context->uid;
            buffer->st_gid = context->gid;
            buffer->st_nlink = 1;
			buffer->st_size = wsfs_json_get_int(data, "size", 0);
			buffer->st_atime = wsfs_json_get_int(data, "atime", 0);
			buffer->st_mtime = wsfs_json_get_int(data, "mtime", 0);
			buffer->st_ctime = wsfs_json_get_int(data, "ctime", 0);
		}
		else
		{
			status = WSFS_BAD_FORMAT;
		}


		json_decref(data);
	}

	return wsfs_status_to_rc(status);
}
