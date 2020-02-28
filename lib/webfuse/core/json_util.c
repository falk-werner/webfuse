#include "webfuse/core/json_util.h"
#include "jsonrpc/status.h"

int wf_impl_json_get_int(json_t const * object, char const * key, int default_value)
{
	int result = default_value;

	json_t * holder = json_object_get(object, key);
	if ((NULL != holder) && (json_is_integer(holder)))
	{
		result = json_integer_value(holder);
	}

	return result;
}

static wf_status wf_impl_jsonrc_code_to_status(int code)
{
	switch (code)
	{
		case JSONRPC_GOOD:
			return WF_GOOD;
		case JSONRPC_BAD:
			return WF_BAD;
		case JSONRPC_BAD_BUSY:
			return WF_BAD_BUSY;
		case JSONRPC_BAD_TIMEOUT:
			return WF_BAD_TIMEOUT;
		case JSONRPC_BAD_FORMAT:
			return WF_BAD_FORMAT;
		default:
			return (wf_status) code;
	}
}

wf_status 
wf_impl_jsonrpc_get_status(
	json_t const * error)
{
	wf_status status = WF_GOOD;
	if (NULL != error)
	{
		int code = wf_impl_json_get_int(error, "code", WF_BAD_FORMAT);
		status = wf_impl_jsonrc_code_to_status(code);
	}

	return status;
}
