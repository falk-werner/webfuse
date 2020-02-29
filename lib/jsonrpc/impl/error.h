#ifndef JSONRPC_IMPL_ERROR_H
#define JSONRPC_IMPL_ERROR_H

#include <jansson.h>
#include "jsonrpc/proxy_finished_fn.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern json_t *
jsonrpc_impl_error(
    int code,
    char const * message);

extern void
jsonrpc_impl_propate_error(
    jsonrpc_proxy_finished_fn * finised,
    void * user_data,
    int code,
    char const * message);

#ifdef __cplusplus
}
#endif


#endif
