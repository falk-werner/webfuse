#ifndef WF_IMPL_UTIL_JSON_UTIL_H
#define WF_IMPL_UTIL_JSON_UTIL_H

#include "webfuse/status.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_jsonrpc_error;
struct wf_json;

extern int
wf_impl_json_get_int(
    struct wf_json const * object,
    char const * key,
    int default_value);

extern wf_status 
wf_impl_jsonrpc_get_status(
    struct wf_jsonrpc_error const * error);

#ifdef __cplusplus
}
#endif

#endif
