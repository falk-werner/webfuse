#include "wsfs/operations.h"

#include <jansson.h>

#include "wsfs/util.h"
#include "wsfs/jsonrpc.h"

int wsfs_operation_readdir(
	char const * path,
	void * buffer,
	fuse_fill_dir_t filler,
	off_t WSFS_UNUSED_PARAM(offset),
	struct fuse_file_info * WSFS_UNUSED_PARAM(file_info),
	enum fuse_readdir_flags WSFS_UNUSED_PARAM(flags))
{
    struct fuse_context * context = fuse_get_context();
	struct wsfs_jsonrpc * rpc = context->private_data;

	json_t * result = NULL;
	wsfs_status const status = wsfs_jsonrpc_invoke(rpc, &result, "readdir", "s", path);
	if (NULL != result)
	{
		if (json_is_array(result)) 
		{
			bool buffer_full = false;
			size_t const count = json_array_size(result);
			for(size_t i = 0; (!buffer_full) && (i < count); i++)
			{
				json_t * entry =json_array_get(result, i);
				if (json_is_object(entry))
				{
					json_t * name_holder = json_object_get(entry, "name");
					if ((NULL != name_holder) && (json_is_string(name_holder))) 
					{
						buffer_full = filler(buffer, json_string_value(name_holder), NULL, 0, 0);
					}
				}
			}
		}

		json_decref(result);
	}
    
    return wsfs_status_to_rc(status);
}