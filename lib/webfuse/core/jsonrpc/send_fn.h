#ifndef WF_JSONRPC_SEND_FN_H
#define WF_JSONRPC_SEND_FN_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include <jansson.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef bool wf_jsonrpc_send_fn(
	json_t * request,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif
