#include "webfuse/adapter/impl/operation/context.h"
#include "webfuse/adapter/impl/session_manager.h"
#include "webfuse/adapter/impl/session.h"
#include <stddef.h>

struct wf_jsonrpc_proxy * wf_impl_operation_context_get_proxy(
	struct wf_impl_operation_context * context)
{
    struct wf_jsonrpc_proxy * proxy = NULL;

    struct wf_impl_session * session = context->session;
    if (NULL != session)
    {
        proxy = session->rpc;
    }

    return proxy;
}
