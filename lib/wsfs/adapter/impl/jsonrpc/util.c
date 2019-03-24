#include "wsfs/adapter/impl/jsonrpc/util.h"

int json_get_int(json_t const * object, char const * key, int default_value)
{
	int result = default_value;

	json_t * holder = json_object_get(object, key);
	if ((NULL != holder) && (json_is_integer(holder)))
	{
		result = json_integer_value(holder);
	}

	return result;
}