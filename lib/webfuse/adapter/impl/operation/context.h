#ifndef WF_ADAPTER_IMPL_OPERATION_CONTEXT_H
#define WF_ADAPTER_IMPL_OPERATION_CONTEXT_H

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

extern struct wf_jsonrpc_proxy * wf_impl_operations_context_get_proxy(
	struct wf_impl_operations_context * context);

#ifdef __cplusplus
}
#endif

#endif

