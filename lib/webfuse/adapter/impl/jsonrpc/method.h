#ifndef WF_ADAPTER_IMPL_JSONRPC_METHOD_H
#define WF_ADAPTER_IMPL_JSONRPC_METHOD_H

#include <jansson.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_impl_jsonrpc_request;

typedef void wf_impl_jsonrpc_method_invoke_fn(
    struct wf_impl_jsonrpc_request * request,
    char const * method_name,
    json_t * params,
    void * user_data);

struct wf_impl_jsonrpc_method
{
    struct wf_impl_jsonrpc_method * next;
    char * name;
    wf_impl_jsonrpc_method_invoke_fn * invoke;
    void * user_data;
};

extern struct wf_impl_jsonrpc_method * wf_impl_jsonrpc_method_create(
    char const * method_name,
    wf_impl_jsonrpc_method_invoke_fn * invoke,
    void * user_data);

extern void wf_impl_jsonrpc_method_dispose(
    struct wf_impl_jsonrpc_method * method);

#ifdef __cplusplus
}
#endif


#endif
