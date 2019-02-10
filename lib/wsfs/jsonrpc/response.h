#ifndef _WSFS_JSONRPC_RESPONSE_H
#define _WFSF_JSONRPC_RESPONSE_H

#ifndef __cplusplus
#include <stddef.h>
#else
#include <cstddef>
using std::size_t;
#endif

#include <jansson.h>
#include "wsfs/status.h"

struct wsfs_jsonrpc_response
{
	wsfs_status status;
	int id;
	json_t * result;	
};

#ifdef __cplusplus
extern "C" {
#endif

extern void wsfs_jsonrpc_response_init(
	struct wsfs_jsonrpc_response * response,
    char const * buffer,
	size_t buffer_length);

extern void wsfs_jsonrpc_response_cleanup(
	struct wsfs_jsonrpc_response * response);

#ifdef __cplusplus
}
#endif

#endif

