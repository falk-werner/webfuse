#ifndef JSONRPC_SERVER_H
#define JSONRPC_SERVER_H

#ifndef __cplusplus
#include <stdarg.h>
#include <stdbool.h>
#else
#include <cstdarg>
#endif

#include <jansson.h>
#include <jsonrpc/api.h>
#include <jsonrpc/send_fn.h>
#include <jsonrpc/method_invoke_fn.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct jsonrpc_server;

extern JSONRPC_API struct jsonrpc_server *
jsonrpc_server_create(void);

extern JSONRPC_API void
jsonrpc_server_dispose(
    struct jsonrpc_server * server);

extern JSONRPC_API void jsonrpc_server_add(
    struct jsonrpc_server * server,
    char const * method_name,
    jsonrpc_method_invoke_fn * invoke,
    void * user_data);

extern JSONRPC_API void jsonrpc_server_process(
    struct jsonrpc_server * server,
    json_t * request,
    jsonrpc_send_fn * send,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif
