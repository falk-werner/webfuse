#include "wsfs/operations.h"


#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> 

#include "wsfs/jsonrpc.h"
#include "wsfs/util.h"

extern void wsfs_operation_ll_getattr (
	fuse_req_t request,
	fuse_ino_t inode,
	struct fuse_file_info * WSFS_UNUSED_PARAM(file_info))
{
    printf("getattr: inode=%lu\n", inode);

    struct fuse_ctx const * context = fuse_req_ctx(request);
    struct wsfs_jsonrpc * rpc = fuse_req_userdata(request);

    struct stat buffer;
	json_t * data = NULL;
	wsfs_status status = wsfs_jsonrpc_invoke(rpc, &data, "getattr", "i", inode);
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

		json_decref(data);
	}

    if (WSFS_GOOD == status)
    {
        fuse_reply_attr(request, &buffer, 1.0);
    }
    else
    {
	    fuse_reply_err(request, ENOENT);
    }
}
