#ifndef WF_IMPL_JSONRPC_METHOD_INVOKE_FN_H
#define WF_IMPL_JSONRPC_METHOD_INVOKE_FN_H

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_jsonrpc_request;
struct wf_json;

typedef void wf_jsonrpc_method_invoke_fn(
    struct wf_jsonrpc_request * request,
    char const * method_name,
    struct wf_json const * params,
    void * user_data);

#ifdef __cplusplus
}
#endif


#endif
