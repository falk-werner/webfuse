#include "wsfs/adapter/jsonrpc/response.h"

void wsfs_jsonrpc_response_init(
	struct wsfs_jsonrpc_response * result,
    char const * buffer,
	size_t length)
{
	result->status = WSFS_BAD;
	result->id = -1;
	result->result = NULL;

	json_t * response = json_loadb(buffer, length, 0, NULL);
	if (NULL == response)
	{
		result->status = WSFS_BAD_FORMAT;
		return;
	}

	json_t * id_holder = json_object_get(response, "id");
	if ((NULL == id_holder) || (!json_is_integer(id_holder)))
	{
		result->status = WSFS_BAD_FORMAT;
		json_decref(response);
		return;
	}
	
	result->status = WSFS_GOOD;
	result->id = json_integer_value(id_holder);
	result->result = json_object_get(response, "result");
	if (NULL != result->result)
	{
		json_incref(result->result);
	}
	else
	{
		result->status = WSFS_BAD_FORMAT;

		json_t * error = json_object_get(response, "error");
		if (NULL != error)
		{
			json_t * error_code = json_object_get(error, "code");
			if ((NULL != error_code) && (json_is_integer(error_code)))
			{
				result->status = json_integer_value(error_code);
			}
		}
	}
	
	json_decref(response);
}

void wsfs_jsonrpc_response_cleanup(
	struct wsfs_jsonrpc_response * response)
{
    if (NULL != response->result)
    {
        json_decref(response->result);
    }
}
