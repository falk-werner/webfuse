#ifndef WF_JSONRPC_REQUEST_H
#define WF_JSONRPC_REQUEST_H

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
#include "webfuse/core/jsonrpc/send_fn.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_jsonrpc_request;

extern bool wf_jsonrpc_is_request(
    json_t * message);

extern struct wf_jsonrpc_request *
wf_jsonrpc_request_create(
    int id,
    wf_jsonrpc_send_fn * send,
    void * user_data);

extern void wf_jsonrpc_request_dispose(
    struct wf_jsonrpc_request * request);

extern void * wf_jsonrpc_request_get_userdata(
    struct wf_jsonrpc_request * request);

extern void wf_jsonrpc_respond(
    struct wf_jsonrpc_request * request,
    json_t * result);

extern void wf_jsonrpc_respond_error(
    struct wf_jsonrpc_request * request,
    int code,
    char const * message);


#ifdef __cplusplus
}
#endif

#endif
