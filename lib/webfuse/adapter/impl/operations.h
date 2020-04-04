#ifndef WF_ADAPTER_IMPL_OPERATIONS_H
#define WF_ADAPTER_IMPL_OPERATIONS_H

#include "webfuse/adapter/impl/fuse_wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

struct wf_impl_session;
struct wf_jsonrpc_proxy;

struct wf_impl_operations_context
{
	struct wf_impl_session * session;
	double timeout;
	char * name;
};

extern void wf_impl_operation_lookup (
	fuse_req_t req, 
	fuse_ino_t parent, 
	char const * name);

extern struct wf_jsonrpc_proxy * wf_impl_operations_context_get_proxy(
	struct wf_impl_operations_context * context);

#ifdef __cplusplus
}
#endif

#endif

