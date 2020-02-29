#ifndef JSONRPC_IMPL_METHOD_H
#define JSONRPC_IMPL_METHOD_H

#include "jsonrpc/method_invoke_fn.h"

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

extern struct jsonrpc_method *
jsonrpc_impl_method_create(
    char const * method_name,
    jsonrpc_method_invoke_fn * invoke,
    void * user_data);

extern void
jsonrpc_impl_method_dispose(
    struct jsonrpc_method * method);

#ifdef __cplusplus
}
#endif


#endif
