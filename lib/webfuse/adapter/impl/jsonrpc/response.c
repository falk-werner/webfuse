#include "webfuse/adapter/impl/jsonrpc/response.h"

extern bool wf_impl_jsonrpc_is_response(
    json_t * message)
{
	json_t * id = json_object_get(message, "id");
	json_t * err = json_object_get(message, "error");
	json_t * result = json_object_get(message, "result");

	return (json_is_integer(id) && 
		(json_is_object(err) ||  (NULL != result)));
}


void wf_impl_jsonrpc_response_init(
	struct wf_impl_jsonrpc_response * result,
	json_t * response)
{
	result->status = WF_BAD;
	result->id = -1;
	result->result = NULL;

	json_t * id_holder = json_object_get(response, "id");
	if ((NULL == id_holder) || (!json_is_integer(id_holder)))
	{
		result->status = WF_BAD_FORMAT;
		return;
	}
	
	result->status = WF_GOOD;
	result->id = json_integer_value(id_holder);
	result->result = json_object_get(response, "result");
	if (NULL != result->result)
	{
		json_incref(result->result);
	}
	else
	{
		result->status = WF_BAD_FORMAT;

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
}

void wf_impl_jsonrpc_response_cleanup(
	struct wf_impl_jsonrpc_response * response)
{
    if (NULL != response->result)
    {
        json_decref(response->result);
    }
}
