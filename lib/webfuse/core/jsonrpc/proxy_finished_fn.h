#ifndef WF_JSONRPC_PROXY_FINISHED_FN_H
#define WF_JSONRPC_PROXY_FINISHED_FN_H

#include <jansson.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef void wf_jsonrpc_proxy_finished_fn(
	void * user_data,
	json_t const * result,
    json_t const * error);

#ifdef __cplusplus
}
#endif

#endif
