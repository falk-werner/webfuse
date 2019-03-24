#ifndef WSFS_ADAPTER_IMPL_JSONRPC_METHOD_H
#define WSFS_ADAPTER_IMPL_JSONRPC_METHOD_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include <jansson.h>
#include "wsfs/status.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef bool wsfs_jsonrpc_method_invoke_fn(
    void * user_data,
    struct json_t const * method_call);

typedef void wsfs_jsonrpc_method_finished_fn(
	void * user_data,
	wsfs_status status,
	struct json_t const * result);

#ifdef __cplusplus
}
#endif



#endif
