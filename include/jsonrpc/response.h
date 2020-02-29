#ifndef JSONRPC_RESPONSE_H
#define JSONRPC_RESPONSE_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include <jansson.h>
#include <jsonrpc/api.h>

#ifdef __cplusplus
extern "C"
{
#endif

extern JSONRPC_API bool jsonrpc_is_response(
    json_t * message);

#ifdef __cplusplus
}
#endif

#endif
