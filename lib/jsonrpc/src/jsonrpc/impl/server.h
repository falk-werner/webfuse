#ifndef JSONRPC_IMPL_SERVER_H
#define JSONRPC_IMPL_SERVER_H

#include <jansson.h>
#include "jsonrpc/method_invoke_fn.h"
#include "jsonrpc/send_fn.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct jsonrpc_server;

extern struct jsonrpc_server *
jsonrpc_impl_server_create(void);

extern void
jsonrpc_impl_server_dispose(
    struct jsonrpc_server * server);

extern void
jsonrpc_impl_server_add(
    struct jsonrpc_server * server,
    char const * method_name,
    jsonrpc_method_invoke_fn * invoke,
    void * user_data);

extern void
jsonrpc_impl_server_process(
    struct jsonrpc_server * server,
    json_t * request,
    jsonrpc_send_fn * send,
    void * user_data);

#ifdef __cplusplus
}
#endif


#endif
