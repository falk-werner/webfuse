#ifndef JSONRPC_ERROR_H
#define JSONRPC_ERROR_H

#include <jansson.h>
#include "jsonrpc/proxy.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern json_t *
jsonrpc_error(
    int code,
    char const * message);

extern void
jsonrpc_propate_error(
    jsonrpc_proxy_finished_fn * finised,
    void * user_data,
    int code,
    char const * message);

#ifdef __cplusplus
}
#endif


#endif
