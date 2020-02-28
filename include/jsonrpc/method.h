#ifndef JSONRPC_METHOD_H
#define JSONRPC_METHOD_H

#include <jansson.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct jsonrpc_request;

typedef void jsonrpc_method_invoke_fn(
    struct jsonrpc_request * request,
    char const * method_name,
    json_t * params,
    void * user_data);

struct jsonrpc_method
{
    struct jsonrpc_method * next;
    char * name;
    jsonrpc_method_invoke_fn * invoke;
    void * user_data;
};

extern struct jsonrpc_method * jsonrpc_method_create(
    char const * method_name,
    jsonrpc_method_invoke_fn * invoke,
    void * user_data);

extern void jsonrpc_method_dispose(
    struct jsonrpc_method * method);

#ifdef __cplusplus
}
#endif


#endif
