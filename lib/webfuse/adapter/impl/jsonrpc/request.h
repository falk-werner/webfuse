#ifndef WF_ADAPTER_IMPL_JSONRPC_REQUEST_H
#define WF_ADAPTER_IMPL_JSONRPC_REQUEST_H

#ifndef __cplusplus
#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>
#else
#include <cstdarg>
#include <cstddef>
using std::size_t;
#endif

#include <jansson.h>
#include "webfuse/core/status.h"
#include "webfuse/adapter/impl/jsonrpc/send_fn.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_impl_jsonrpc_request;

extern struct wf_impl_jsonrpc_request * wf_impl_jsonrpc_request_create(
    int id,
    wf_impl_jsonrpc_send_fn * send,
    void * user_data);

extern void wf_impl_jsonrpc_request_dispose(
    struct wf_impl_jsonrpc_request * request);

extern void wf_impl_jsonrpc_respond(
    struct wf_impl_jsonrpc_request * request,
    json_t * result);

extern void wf_impl_jsonrpc_respond_error(
    struct wf_impl_jsonrpc_request * request,
    wf_status status);


#ifdef __cplusplus
}
#endif

#endif
