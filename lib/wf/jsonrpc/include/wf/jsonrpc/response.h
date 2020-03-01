#ifndef WF_JSONRPC_RESPONSE_H
#define WF_JSONRPC_RESPONSE_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include <jansson.h>
#include <wf/jsonrpc/api.h>

#ifdef __cplusplus
extern "C"
{
#endif

extern WF_JSONRPC_API bool wf_jsonrpc_is_response(
    json_t * message);

#ifdef __cplusplus
}
#endif

#endif
