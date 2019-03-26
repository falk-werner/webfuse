#ifndef WF_ADAPTER_IMPL_JSONRPC_RESPONSE_H
#define WF_ADAPTER_IMPL_JSONRPC_RESPONSE_H

#ifndef __cplusplus
#include <stddef.h>
#else
#include <cstddef>
using std::size_t;
#endif

#include <jansson.h>
#include "webfuse/core/status.h"

#ifdef __cplusplus
extern "C" {
#endif

struct wf_impl_jsonrpc_response
{
	wf_status status;
	int id;
	json_t * result;	
};

extern void wf_impl_jsonrpc_response_init(
	struct wf_impl_jsonrpc_response * response,
    char const * buffer,
	size_t buffer_length);

extern void wf_impl_jsonrpc_response_cleanup(
	struct wf_impl_jsonrpc_response * response);

#ifdef __cplusplus
}
#endif

#endif

