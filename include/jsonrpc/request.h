#ifndef JSONRPC_REQUEST_H
#define JSONRPC_REQUEST_H

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
#include "jsonrpc/send_fn.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct jsonrpc_request;

extern bool jsonrpc_is_request(
    json_t * message);

extern struct jsonrpc_request * jsonrpc_request_create(
    int id,
    jsonrpc_send_fn * send,
    void * user_data);

extern void jsonrpc_request_dispose(
    struct jsonrpc_request * request);

extern void * jsonrpc_request_get_userdata(
    struct jsonrpc_request * request);

extern void jsonrpc_respond(
    struct jsonrpc_request * request,
    json_t * result);

extern void jsonrpc_respond_error(
    struct jsonrpc_request * request,
    wf_status status);


#ifdef __cplusplus
}
#endif

#endif
