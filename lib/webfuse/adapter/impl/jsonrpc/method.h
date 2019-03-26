#ifndef WF_ADAPTER_IMPL_JSONRPC_METHOD_H
#define WF_ADAPTER_IMPL_JSONRPC_METHOD_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include <jansson.h>
#include "webfuse/core/status.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef bool wf_impl_jsonrpc_method_invoke_fn(
    void * user_data,
    struct json_t const * method_call);

typedef void wf_impl_jsonrpc_method_finished_fn(
	void * user_data,
	wf_status status,
	struct json_t const * result);

#ifdef __cplusplus
}
#endif



#endif
