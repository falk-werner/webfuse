#include "wsfs/jsonrpc/method_intern.h"

#include <stdlib.h>
#include <string.h>

struct wsfs_jsonrpc_method * wsfs_jsonrpc_method_create(
    char const * name,
    wsfs_jsonrpc_method_invoke_fn * invoke,
    void * user_data)
{
    struct wsfs_jsonrpc_method * method = malloc(sizeof(struct wsfs_jsonrpc_method));
    if (NULL != method)
    {
        method->next = NULL;
        method->name = strdup(name);
        method->invoke = invoke;
        method->user_data = user_data;
    }

    return method;
}

void wsfs_jsonrpc_method_dispose(
    struct wsfs_jsonrpc_method * method)
{
    free(method->name);
    free(method);
}
