#ifndef WF_IMPL_JSONRPC_ERROR_H
#define WF_IMPL_JSONRPC_ERROR_H

#include <jansson.h>
#include "webfuse/impl/jsonrpc/proxy_finished_fn.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern json_t *
wf_impl_jsonrpc_error(
    int code,
    char const * message);

extern void
wf_impl_jsonrpc_propate_error(
    wf_jsonrpc_proxy_finished_fn * finised,
    void * user_data,
    int code,
    char const * message);

#ifdef __cplusplus
}
#endif


#endif
