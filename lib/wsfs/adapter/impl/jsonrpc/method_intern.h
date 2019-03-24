#ifndef WSFS_ADAPTER_IMPL_JSONRPC_METHOD_INTERN_H
#define WSFS_ADAPTER_IMPL_JSONRPC_METHOD_INTERN_H

#include "wsfs/adapter/impl/jsonrpc/method.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct jsonrpc_method
{
    struct jsonrpc_method * next;
    char * name;
    jsonrpc_method_invoke_fn * invoke;
    void * user_data;
};

extern struct jsonrpc_method * jsonrpc_method_create(
    char const * name,
    jsonrpc_method_invoke_fn * invoke,
    void * user_data);

extern void jsonrpc_method_dispose(
    struct jsonrpc_method * method);

#ifdef __cplusplus
}
#endif

#endif
