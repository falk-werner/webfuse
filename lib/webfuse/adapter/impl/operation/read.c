#include "webfuse/adapter/impl/operations.h"

#include <errno.h>
#include <string.h>
#include <limits.h>
#include <jansson.h>

#include "wf/jsonrpc/proxy.h"
#include "webfuse/core/base64.h"
#include "webfuse/core/json_util.h"

#define WF_MAX_READ_LENGTH 4096

static char * wf_impl_fill_buffer(
	char const * data,
	char const * format,
	size_t count,
	wf_status * status)
{
	*status = WF_GOOD;
	char * buffer = malloc(count);

	if (0 < count)
	{
		if (0 == strcmp("identity", format))
		{
			memcpy(buffer, data, count);
		}
		else if (0 == strcmp("base64", format))
		{
			wf_base64_decode(data, strlen(data), (uint8_t *) buffer, count);
		}
		else
		{
			*status = WF_BAD;
		}
	}

	return buffer;
}

static void wf_impl_operation_read_finished(
	void * user_data, 
	json_t const * result,
	json_t const * error)
{
	wf_status status = wf_impl_jsonrpc_get_status(error);
	fuse_req_t request = user_data;

	char * buffer = NULL;	
	size_t length = 0;
	if (NULL != result)
	{
		json_t * data_holder = json_object_get(result, "data");
		json_t * format_holder = json_object_get(result, "format");
		json_t * count_holder = json_object_get(result, "count");

		if (json_is_string(data_holder) &&
        	json_is_string(format_holder) &&
            json_is_integer(count_holder))
		{
			char const * const data = json_string_value(data_holder);
			char const * const format = json_string_value(format_holder);
			length = (size_t) json_integer_value(count_holder);

			buffer = wf_impl_fill_buffer(data, format, length, &status);
		}
		else
		{
			status = WF_BAD_FORMAT;
		}
	}

	if (WF_GOOD == status)
	{
		fuse_reply_buf(request, buffer, length);
	}
	else
	{
   		fuse_reply_err(request, ENOENT);
	}
	
    free(buffer);

}

void wf_impl_operation_read(
	fuse_req_t request,
	fuse_ino_t inode,
    size_t size,
    off_t offset,
	struct fuse_file_info * file_info)
{
    struct wf_impl_operations_context * user_data = fuse_req_userdata(request);
    struct wf_jsonrpc_proxy * rpc = wf_impl_operations_context_get_proxy(user_data);

	if (NULL != rpc)
	{
		int const length = (size <= WF_MAX_READ_LENGTH) ? (int) size : WF_MAX_READ_LENGTH;
		int handle = (file_info->fh & INT_MAX);
		wf_jsonrpc_proxy_invoke(rpc, &wf_impl_operation_read_finished, request, "read", "siiii", user_data->name, (int) inode, handle, (int) offset, length);
	}
	else
	{
		fuse_reply_err(request, ENOENT);
	}	
}
