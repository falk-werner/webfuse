#include "webfuse/impl/jsonrpc/response_intern.h"
#include "webfuse/impl/jsonrpc/error.h"
#include "webfuse/status.h"

bool
wf_impl_jsonrpc_is_response(
    json_t * message)
{
	json_t * id = json_object_get(message, "id");
	json_t * err = json_object_get(message, "error");
	json_t * result = json_object_get(message, "result");

	return (json_is_integer(id) && 
		(json_is_object(err) ||  (NULL != result)));
}


void
wf_impl_jsonrpc_response_init(
	struct wf_jsonrpc_response * result,
	json_t * response)
{
	result->id = -1;
	result->result = NULL;
	result->error = NULL;

	json_t * id_holder = json_object_get(response, "id");
	if (!json_is_integer(id_holder))
	{
		result->error = wf_impl_jsonrpc_error(WF_BAD_FORMAT, "invalid format: missing id");
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
		int code = WF_BAD_FORMAT;
		char const * message = "invalid format: invalid error object";

		json_t * error = json_object_get(response, "error");
		json_t * code_holder = json_object_get(error, "code");
		if (json_is_integer(code_holder))
		{
			code = json_integer_value(json_object_get(error, "code"));
			json_t * message_holder = json_object_get(error, "message");
			message = json_is_string(message_holder) ? json_string_value(message_holder) : "";
		}

		result->error = wf_impl_jsonrpc_error(code, message);
	}
}

void
wf_impl_jsonrpc_response_cleanup(
	struct wf_jsonrpc_response * response)
{
    if (NULL != response->result)
    {
        json_decref(response->result);
    }

	wf_impl_jsonrpc_error_dispose(response->error);
}
