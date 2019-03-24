#include "wsfs/adapter/impl/jsonrpc/method_intern.h"

#include <stdlib.h>
#include <string.h>

struct jsonrpc_method * jsonrpc_method_create(
    char const * name,
    jsonrpc_method_invoke_fn * invoke,
    void * user_data)
{
    struct jsonrpc_method * method = malloc(sizeof(struct jsonrpc_method));
    if (NULL != method)
    {
        method->next = NULL;
        method->name = strdup(name);
        method->invoke = invoke;
        method->user_data = user_data;
    }

    return method;
}

void jsonrpc_method_dispose(
    struct jsonrpc_method * method)
{
    free(method->name);
    free(method);
}
