#ifndef WF_IMPL_JSONRPC_RESPONSE_H
#define WF_IMPL_JSONRPC_RESPONSE_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_json;

extern bool wf_impl_jsonrpc_is_response(
    struct wf_json const  * message);

#ifdef __cplusplus
}
#endif

#endif
