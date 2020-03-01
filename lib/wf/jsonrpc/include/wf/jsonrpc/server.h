#ifndef WF_JSONRPC_SERVER_H
#define WF_JSONRPC_SERVER_H

#ifndef __cplusplus
#include <stdarg.h>
#include <stdbool.h>
#else
#include <cstdarg>
#endif

#include <jansson.h>
#include <wf/jsonrpc/api.h>
#include <wf/jsonrpc/send_fn.h>
#include <wf/jsonrpc/method_invoke_fn.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_jsonrpc_server;

extern WF_JSONRPC_API struct wf_jsonrpc_server *
wf_jsonrpc_server_create(void);

extern WF_JSONRPC_API void
wf_jsonrpc_server_dispose(
    struct wf_jsonrpc_server * server);

extern WF_JSONRPC_API void wf_jsonrpc_server_add(
    struct wf_jsonrpc_server * server,
    char const * method_name,
    wf_jsonrpc_method_invoke_fn * invoke,
    void * user_data);

extern WF_JSONRPC_API void wf_jsonrpc_server_process(
    struct wf_jsonrpc_server * server,
    json_t * request,
    wf_jsonrpc_send_fn * send,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif
