#include "webfuse/impl/util/json_util.h"
#include "webfuse/impl/jsonrpc/error.h"
#include "webfuse/impl/json/node.h"

int
wf_impl_json_get_int(
	struct wf_json const * object,
	char const * key,
	int default_value)
{
	int result = default_value;

	struct wf_json const * holder = wf_impl_json_object_get(object, key);
	if (wf_impl_json_is_int(holder))
	{
		result = wf_impl_json_int_get(holder);
	}

	return result;
}

wf_status 
wf_impl_jsonrpc_get_status(
	struct wf_jsonrpc_error const * error)
{
	wf_status status = WF_GOOD;
	if (NULL != error)
	{
		status = wf_impl_jsonrpc_error_code(error);
	}

	return status;
}
