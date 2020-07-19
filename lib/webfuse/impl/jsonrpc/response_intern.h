#ifndef WF_IMPL_JSONRPC_RESPONSE_INTERN_H
#define WF_IMPL_JSONRPC_RESPONSE_INTERN_H

#include "webfuse/impl/jsonrpc/response.h"

#ifndef __cplusplus
#include <stddef.h>
#else
#include <cstddef>
using std::size_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct wf_jsonrpc_error;
struct wf_json;

struct wf_jsonrpc_response
{
	struct wf_json const * result;	
	struct wf_jsonrpc_error * error;
	int id;
};

extern void wf_impl_jsonrpc_response_init(
	struct wf_jsonrpc_response * response,
    struct wf_json const * message);

extern void wf_impl_jsonrpc_response_cleanup(
	struct wf_jsonrpc_response * response);

#ifdef __cplusplus
}
#endif

#endif

