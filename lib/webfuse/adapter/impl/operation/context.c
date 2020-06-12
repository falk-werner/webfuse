#include "webfuse/adapter/impl/operation/context.h"
#include "webfuse/adapter/impl/session_manager.h"
#include "webfuse/adapter/impl/session.h"
#include <stddef.h>

struct wf_jsonrpc_proxy * wf_impl_operation_context_get_proxy(
	struct wf_impl_operation_context * context)
{
    return context->proxy;
}
