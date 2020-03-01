#include "wf/jsonrpc/impl/response.h"
#include "wf/jsonrpc/impl/error.h"
#include "wf/jsonrpc/status.h"

bool
wf_jsonrpc_impl_is_response(
    json_t * message)
{
	json_t * id = json_object_get(message, "id");
	json_t * err = json_object_get(message, "error");
	json_t * result = json_object_get(message, "result");

	return (json_is_integer(id) && 
		(json_is_object(err) ||  (NULL != result)));
}


void
wf_jsonrpc_impl_response_init(
	struct wf_jsonrpc_response * result,
	json_t * response)
{
	result->id = -1;
	result->result = NULL;
	result->error = NULL;

	json_t * id_holder = json_object_get(response, "id");
	if ((NULL == id_holder) || (!json_is_integer(id_holder)))
	{
		result->error = wf_jsonrpc_impl_error(WF_JSONRPC_BAD_FORMAT, "invalid format: missing id");
		return;
	}
	
	result->id = json_integer_value(id_holder);
	result->result = json_object_get(response, "result");
	if (NULL != result->result)
	{
		json_incref(result->result);
	}
	else
	{
		json_t * error = json_object_get(response, "error");
		if ((json_is_object(error)) && (json_is_integer(json_object_get(error, "code"))))
		{
			result->error = error;
			json_incref(result->error);
		}
		else
		{
			result->error = wf_jsonrpc_impl_error(WF_JSONRPC_BAD_FORMAT, "invalid format: invalid error object");
		}
	}
}

void
wf_jsonrpc_impl_response_cleanup(
	struct wf_jsonrpc_response * response)
{
    if (NULL != response->result)
    {
        json_decref(response->result);
    }

    if (NULL != response->error)
    {
        json_decref(response->error);
    }
}
