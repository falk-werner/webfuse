#ifndef WF_ADAPTER_IMPL_JSONRPC_REQUEST_H
#define WF_ADAPTER_IMPL_JSONRPC_REQUEST_H

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

extern json_t * wf_impl_jsonrpc_request_create(
	char const * method,
	int id,
	char const * param_info,
	va_list args);

#ifdef __cplusplus
}
#endif

#endif