#ifndef WF_JSONRPC_IMPL_SERVER_H
#define WF_JSONRPC_IMPL_SERVER_H

#include <jansson.h>
#include "wf/jsonrpc/method_invoke_fn.h"
#include "wf/jsonrpc/send_fn.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_jsonrpc_server;

extern struct wf_jsonrpc_server *
wf_jsonrpc_impl_server_create(void);

extern void
wf_jsonrpc_impl_server_dispose(
    struct wf_jsonrpc_server * server);

extern void
wf_jsonrpc_impl_server_add(
    struct wf_jsonrpc_server * server,
    char const * method_name,
    wf_jsonrpc_method_invoke_fn * invoke,
    void * user_data);

extern void
wf_jsonrpc_impl_server_process(
    struct wf_jsonrpc_server * server,
    json_t * request,
    wf_jsonrpc_send_fn * send,
    void * user_data);

#ifdef __cplusplus
}
#endif


#endif
