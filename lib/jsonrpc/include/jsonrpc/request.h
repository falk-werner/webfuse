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
#include <jsonrpc/api.h>
#include "jsonrpc/send_fn.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct jsonrpc_request;

extern JSONRPC_API bool jsonrpc_is_request(
    json_t * message);

extern JSONRPC_API struct jsonrpc_request * jsonrpc_request_create(
    int id,
    jsonrpc_send_fn * send,
    void * user_data);

extern JSONRPC_API void jsonrpc_request_dispose(
    struct jsonrpc_request * request);

extern JSONRPC_API void * jsonrpc_request_get_userdata(
    struct jsonrpc_request * request);

extern JSONRPC_API void jsonrpc_respond(
    struct jsonrpc_request * request,
    json_t * result);

extern JSONRPC_API void jsonrpc_respond_error(
    struct jsonrpc_request * request,
    int code,
    char const * message);


#ifdef __cplusplus
}
#endif

#endif
