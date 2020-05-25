#ifndef WF_JSONRPC_RESPONSE_INTERN_H
#define WF_JSONRPC_RESPONSE_INTERN_H

#include "webfuse/core/jsonrpc/response.h"

#ifndef __cplusplus
#include <stddef.h>
#else
#include <cstddef>
using std::size_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct wf_jsonrpc_response
{
	json_t * result;	
	json_t * error;
	int id;
};

extern void wf_jsonrpc_response_init(
	struct wf_jsonrpc_response * response,
    json_t * message);

extern void wf_jsonrpc_response_cleanup(
	struct wf_jsonrpc_response * response);

#ifdef __cplusplus
}
#endif

#endif

