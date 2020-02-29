#ifndef JSONRPC_METHOD_INVOKE_FN_H
#define JSONRPC_METHOD_INVOKE_FN_H

#include <jsonrpc/api.h>
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


#ifdef __cplusplus
}
#endif


#endif
