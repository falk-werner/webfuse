#ifndef WF_ADAPTER_IMPL_OPERATION_CONTEXT_H
#define WF_ADAPTER_IMPL_OPERATION_CONTEXT_H

#include "webfuse/adapter/impl/fuse_wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

struct wf_jsonrpc_proxy;

struct wf_impl_operation_context
{
	struct wf_jsonrpc_proxy * proxy;
	double timeout;
	char * name;
};

extern struct wf_jsonrpc_proxy * wf_impl_operation_context_get_proxy(
	struct wf_impl_operation_context * context);

#ifdef __cplusplus
}
#endif

#endif

