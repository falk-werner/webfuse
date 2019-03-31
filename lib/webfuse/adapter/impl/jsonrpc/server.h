#ifndef WF_ADAPTER_IMPL_JSONRPC_SERVER_H
#define WF_ADAPTER_IMPL_JSONRPC_SERVER_H

#ifndef __cplusplus
#include <stdarg.h>
#include <stdbool.h>
#else
#include <cstdarg>
#endif

#include <jansson.h>
#include "webfuse/core/status.h"
#include "webfuse/adapter/impl/jsonrpc/send_fn.h"
#include "webfuse/adapter/impl/jsonrpc/method.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_impl_jsonrpc_server
{
    struct wf_impl_jsonrpc_method * methods;
};

extern void wf_impl_jsonrpc_server_init(
    struct wf_impl_jsonrpc_server * server);

extern void wf_impl_jsonrpc_server_cleanup(
    struct wf_impl_jsonrpc_server * server);

extern void wf_impl_jsonrpc_server_add(
    struct wf_impl_jsonrpc_server * server,
    char const * method_name,
    wf_impl_jsonrpc_method_invoke_fn * invoke,
    void * user_data);

extern void wf_impl_jsonrpc_server_process(
    struct wf_impl_jsonrpc_server * server,
    json_t * request,
    wf_impl_jsonrpc_send_fn * send,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif
