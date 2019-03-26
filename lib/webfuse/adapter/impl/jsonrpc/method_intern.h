#ifndef WF_ADAPTER_IMPL_JSONRPC_METHOD_INTERN_H
#define WF_ADAPTER_IMPL_JSONRPC_METHOD_INTERN_H

#include "webfuse/adapter/impl/jsonrpc/method.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_impl_jsonrpc_method
{
    struct wf_impl_jsonrpc_method * next;
    char * name;
    wf_impl_jsonrpc_method_invoke_fn * invoke;
    void * user_data;
};

extern struct wf_impl_jsonrpc_method * wf_impl_jsonrpc_method_create(
    char const * name,
    wf_impl_jsonrpc_method_invoke_fn * invoke,
    void * user_data);

extern void wf_impl_jsonrpc_method_dispose(
    struct wf_impl_jsonrpc_method * method);

#ifdef __cplusplus
}
#endif

#endif
