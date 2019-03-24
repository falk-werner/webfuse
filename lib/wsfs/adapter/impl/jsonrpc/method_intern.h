#ifndef WSFS_ADAPTER_IMPL_JSONRPC_METHOD_INTERN_H
#define WSFS_ADAPTER_IMPL_JSONRPC_METHOD_INTERN_H

#include "wsfs/adapter/impl/jsonrpc/method.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wsfs_impl_jsonrpc_method
{
    struct wsfs_impl_jsonrpc_method * next;
    char * name;
    wsfs_impl_jsonrpc_method_invoke_fn * invoke;
    void * user_data;
};

extern struct wsfs_impl_jsonrpc_method * wsfs_impl_jsonrpc_method_create(
    char const * name,
    wsfs_impl_jsonrpc_method_invoke_fn * invoke,
    void * user_data);

extern void wsfs_impl_jsonrpc_method_dispose(
    struct wsfs_impl_jsonrpc_method * method);

#ifdef __cplusplus
}
#endif

#endif
