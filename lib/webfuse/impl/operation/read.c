#include "webfuse/impl/operation/read.h"
#include "webfuse/impl/operation/context.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "webfuse/impl/jsonrpc/proxy.h"
#include "webfuse/impl/json/node.h"
#include "webfuse/impl/util/base64.h"
#include "webfuse/impl/util/json_util.h"

// do not read chunks larger than 1 MByte
#define WF_MAX_READ_LENGTH (1024 * 1024)

char * wf_impl_fill_buffer(
	char const * data,
	size_t data_size,
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
			if (count == data_size)
			{
				memcpy(buffer, data, count);
			}
			else
			{
				*status = WF_BAD;
			}			
		}
		else if (0 == strcmp("base64", format))
		{
			size_t result = wf_impl_base64_decode(data, data_size, (uint8_t *) buffer, count);
			if (result != count)
			{
				*status = WF_BAD;
			}
		}
		else
		{
			*status = WF_BAD;
		}
	}

	if (WF_GOOD != *status)
	{
		free(buffer);
		buffer = NULL;
	}

	return buffer;
}

void wf_impl_operation_read_finished(
	void * user_data, 
	struct wf_json const * result,
	struct wf_jsonrpc_error const * error)
{
	wf_status status = wf_impl_jsonrpc_get_status(error);
	fuse_req_t request = user_data;

	char * buffer = NULL;	
	size_t length = 0;
	if (NULL != result)
	{
		struct wf_json const * data_holder = wf_impl_json_object_get(result, "data");
		struct wf_json const * format_holder = wf_impl_json_object_get(result, "format");
		struct wf_json const * count_holder = wf_impl_json_object_get(result, "count");

		if ((WF_JSON_TYPE_STRING == wf_impl_json_type(data_holder)) &&
        	(WF_JSON_TYPE_STRING == wf_impl_json_type(format_holder)) &&
            (WF_JSON_TYPE_INT == wf_impl_json_type(count_holder)))
		{
			char const * const data = wf_impl_json_string_get(data_holder);
			size_t const data_size = wf_impl_json_string_size(data_holder);
			char const * const format = wf_impl_json_string_get(format_holder);
			length = (size_t) wf_impl_json_int_get(count_holder);

			buffer = wf_impl_fill_buffer(data, data_size, format, length, &status);
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
    struct wf_impl_operation_context * user_data = fuse_req_userdata(request);
    struct wf_jsonrpc_proxy * rpc = wf_impl_operation_context_get_proxy(user_data);

	if ((NULL != rpc) && (size <= WF_MAX_READ_LENGTH))
	{
		int const length = (int) size;
		int handle = (file_info->fh & INT_MAX);
		wf_impl_jsonrpc_proxy_invoke(rpc, &wf_impl_operation_read_finished, request, "read", "siiii", user_data->name, (int) inode, handle, (int) offset, length);
	}
	else if (size > WF_MAX_READ_LENGTH)
	{
		fuse_reply_err(request, ENOMEM);
	}
	else
	{
		fuse_reply_err(request, ENOENT);
	}	
}
