#include "webfuse/core/json_util.h"

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

wf_status 
wf_impl_jsonrpc_get_status(
	json_t const * error)
{
	wf_status status = WF_GOOD;
	if (NULL != error)
	{
		status = wf_impl_json_get_int(error, "code", WF_BAD_FORMAT);
	}

	return status;
}
