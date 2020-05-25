#ifndef WF_JSONRPC_METHOD_INVOKE_FN_H
#define WF_JSONRPC_METHOD_INVOKE_FN_H

#include <jansson.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_jsonrpc_request;

typedef void wf_jsonrpc_method_invoke_fn(
    struct wf_jsonrpc_request * request,
    char const * method_name,
    json_t * params,
    void * user_data);

#ifdef __cplusplus
}
#endif


#endif
