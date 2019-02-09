#include "wsfs/jsonrpc/request.h"

json_t * wsfs_jsonrpc_request_create(
	char const * method,
	int id,
	char const * param_info,
	va_list args)
{
	json_t * request = json_object();
	json_object_set_new(request, "method", json_string(method));
	json_t * params = json_array();
	
	for (char const * param_type = param_info; '\0' != *param_type; param_type++)
	{
		switch(*param_type)
		{
			case 's':
			{
				char const * const value = va_arg(args, char const *);
				json_array_append_new(params, json_string(value));
			}
			break;
			case 'i':
			{
				int const value = va_arg(args, int);
				json_array_append_new(params, json_integer(value));
			}
			break;
			default:
			fprintf(stderr, "fatal: unknown param_type '%c'\n", *param_type);
			exit(EXIT_FAILURE);
			break;
		}
	}
	

	json_object_set_new(request, "params", params);
	json_object_set_new(request, "id", json_integer(id));

	return request;
}
