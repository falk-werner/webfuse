#ifndef JSONRPC_IMPL_REQUEST_H
#define JSONRPC_IMPL_REQUEST_H

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
#include <jsonrpc/api.h>
#include "jsonrpc/send_fn.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct jsonrpc_request;

extern bool jsonrpc_impl_is_request(
    json_t * message);

extern struct jsonrpc_request * jsonrpc_impl_request_create(
    int id,
    jsonrpc_send_fn * send,
    void * user_data);

extern void jsonrpc_impl_request_dispose(
    struct jsonrpc_request * request);

extern void * jsonrpc_impl_request_get_userdata(
    struct jsonrpc_request * request);

extern void jsonrpc_impl_respond(
    struct jsonrpc_request * request,
    json_t * result);

extern void jsonrpc_impl_respond_error(
    struct jsonrpc_request * request,
    int code,
    char const * message);


#ifdef __cplusplus
}
#endif

#endif
