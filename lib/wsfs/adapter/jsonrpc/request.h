#ifndef WSFS_ADAPTER_JSONRPC_REQUEST_H
#define WSFS_ADAPTER_JSONRPC_REQUEST_H

#ifndef __cplusplus
#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>
#else
#include <cstdarg>
#include <cstddef>
using std::size_t;
#endif

#include <jansson.h>

#ifdef __cplusplus
extern "C"
{
#endif

extern json_t * wsfs_jsonrpc_request_create(
	char const * method,
	int id,
	char const * param_info,
	va_list args);

#ifdef __cplusplus
}
#endif

#endif
