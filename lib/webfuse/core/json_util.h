#ifndef WF_JSON_UTIL_H
#define WF_JSON_UTIL_H

#include <jansson.h>
#include "webfuse/core/status.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern int
wf_impl_json_get_int(
    json_t const * object,
    char const * key,
    int default_value);

extern wf_status 
wf_impl_jsonrpc_get_status(
    json_t const * error);

#ifdef __cplusplus
}
#endif

#endif
