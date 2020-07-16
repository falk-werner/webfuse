#include "webfuse/impl/jsonrpc/response_intern.h"
#include "webfuse/impl/jsonrpc/error.h"
#include "webfuse/impl/json/node.h"
#include "webfuse/status.h"

bool
wf_impl_jsonrpc_is_response(
    struct wf_json const * message)
{
	struct wf_json const * id = wf_impl_json_object_get(message, "id");
	struct wf_json const * err = wf_impl_json_object_get(message, "error");
	struct wf_json const * result = wf_impl_json_object_get(message, "result");

	return ((WF_JSON_TYPE_INT == wf_impl_json_type(id)) && 
		((WF_JSON_TYPE_OBJECT == wf_impl_json_type(err)) ||  (WF_JSON_TYPE_UNDEFINED != wf_impl_json_type(result))));
}


void
wf_impl_jsonrpc_response_init(
	struct wf_jsonrpc_response * result,
	struct wf_json const * response)
{
	result->id = -1;
	result->result = NULL;
	result->error = NULL;

	struct wf_json const * id_holder = wf_impl_json_object_get(response, "id");
	if (WF_JSON_TYPE_INT != wf_impl_json_type(id_holder))
	{
		result->error = wf_impl_jsonrpc_error(WF_BAD_FORMAT, "invalid format: missing id");
		return;
	}
	
	result->id = wf_impl_json_int_get(id_holder);
	result->result = wf_impl_json_object_get(response, "result");
	if (WF_JSON_TYPE_UNDEFINED == wf_impl_json_type(result->result))
	{
		int code = WF_BAD_FORMAT;
		char const * message = "invalid format: invalid error object";

		struct wf_json const * error = wf_impl_json_object_get(response, "error");
		struct wf_json const * code_holder = wf_impl_json_object_get(error, "code");
		if (WF_JSON_TYPE_INT == wf_impl_json_type(code_holder))
		{
			code = wf_impl_json_int_get(code_holder);
			struct wf_json const * message_holder = wf_impl_json_object_get(error, "message");
			message = wf_impl_json_string_get(message_holder);
		}

		result->error = wf_impl_jsonrpc_error(code, message);
	}
}

void
wf_impl_jsonrpc_response_cleanup(
	struct wf_jsonrpc_response * response)
{
	wf_impl_jsonrpc_error_dispose(response->error);
}
