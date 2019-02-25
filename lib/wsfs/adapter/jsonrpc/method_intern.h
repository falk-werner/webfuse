#ifndef WSFS_JSONRPC_METHOD_INTERN_H
#define WSFS_JSONRPC_METHOD_INTERN_H

#include "wsfs/adapter/jsonrpc/method.h"

struct wsfs_jsonrpc_method
{
    struct wsfs_jsonrpc_method * next;
    char * name;
    wsfs_jsonrpc_method_invoke_fn * invoke;
    void * user_data;
};

#ifdef __cplusplus
extern "C"
{
#endif

extern struct wsfs_jsonrpc_method * wsfs_jsonrpc_method_create(
    char const * name,
    wsfs_jsonrpc_method_invoke_fn * invoke,
    void * user_data);

extern void wsfs_jsonrpc_method_dispose(
    struct wsfs_jsonrpc_method * method);

#ifdef __cplusplus
}
#endif

#endif
