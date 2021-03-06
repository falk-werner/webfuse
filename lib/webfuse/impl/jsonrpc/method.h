#ifndef WF_IMPL_JSONRPC_METHOD_H
#define WF_IMPL_JSONRPC_METHOD_H

#include "webfuse/impl/jsonrpc/method_invoke_fn.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_jsonrpc_method
{
    struct wf_jsonrpc_method * next;
    char * name;
    wf_jsonrpc_method_invoke_fn * invoke;
    void * user_data;
};

extern struct wf_jsonrpc_method *
wf_impl_jsonrpc_method_create(
    char const * method_name,
    wf_jsonrpc_method_invoke_fn * invoke,
    void * user_data);

extern void
wf_impl_jsonrpc_method_dispose(
    struct wf_jsonrpc_method * method);

#ifdef __cplusplus
}
#endif


#endif
