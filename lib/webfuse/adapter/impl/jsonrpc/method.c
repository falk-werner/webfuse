#include "webfuse/adapter/impl/jsonrpc/method_intern.h"

#include <stdlib.h>
#include <string.h>

struct wf_impl_jsonrpc_method * wf_impl_jsonrpc_method_create(
    char const * name,
    wf_impl_jsonrpc_method_invoke_fn * invoke,
    void * user_data)
{
    struct wf_impl_jsonrpc_method * method = malloc(sizeof(struct wf_impl_jsonrpc_method));
    if (NULL != method)
    {
        method->next = NULL;
        method->name = strdup(name);
        method->invoke = invoke;
        method->user_data = user_data;
    }

    return method;
}

void wf_impl_jsonrpc_method_dispose(
    struct wf_impl_jsonrpc_method * method)
{
    free(method->name);
    free(method);
}
