#include "wsfs/adapter/impl/operations.h"

#include <errno.h>
#include <string.h>
#include <limits.h>
#include <jansson.h>
#include <libwebsockets.h>

#include "wsfs/adapter/impl/jsonrpc/server.h"

#define WSFS_MAX_READ_LENGTH 4096

static char * wsfs_fill_buffer(
	char const * data,
	char const * format,
	size_t count,
	wsfs_status * status)
{
	*status = WSFS_GOOD;
	char * buffer = malloc(count + 1);

	if ((NULL != buffer) && (0 < count))
	{
		if (0 == strcmp("identity", format))
		{
			memcpy(buffer, data, count); /* Flawfinder: ignore */
		}
		else if (0 == strcmp("base64", format))
		{
			lws_b64_decode_string(data, buffer, count + 1);
		}
		else
		{
			*status = WSFS_BAD;
		}
	}

	return buffer;
}

static void wsfs_operation_read_finished(void * user_data, wsfs_status status, json_t const * data)
{
	fuse_req_t request = user_data;

	char * buffer = NULL;	
	size_t length = 0;
	if (NULL != data)
	{
		json_t * data_holder = json_object_get(data, "data");
		json_t * format_holder = json_object_get(data, "format");
		json_t * count_holder = json_object_get(data, "count");

		if (json_is_string(data_holder) &&
        	json_is_string(format_holder) &&
            json_is_integer(count_holder))
		{
			char const * const data = json_string_value(data_holder);
			char const * const format = json_string_value(format_holder);
			length = (size_t) json_integer_value(count_holder);

			buffer = wsfs_fill_buffer(data, format, length, &status);
		}
		else
		{
			status = WSFS_BAD_FORMAT;
		}
	}

	if (WSFS_GOOD == status)
	{
		fuse_reply_buf(request, buffer, length);
	}
	else
	{
   		fuse_reply_err(request, ENOENT);
	}
	
    free(buffer);

}

void wsfs_operation_read(
	fuse_req_t request,
	fuse_ino_t inode,
    size_t size,
    off_t offset,
	struct fuse_file_info * file_info)
{
    struct wsfs_operations_context * user_data = fuse_req_userdata(request);
    struct wsfs_jsonrpc_server * rpc = user_data->rpc;

	int const length = (size <= WSFS_MAX_READ_LENGTH) ? (int) size : WSFS_MAX_READ_LENGTH;
    int handle = (file_info->fh & INT_MAX);
	wsfs_jsonrpc_server_invoke(rpc, &wsfs_operation_read_finished, request, "read", "iiii", inode, handle, (int) offset, length);
}
