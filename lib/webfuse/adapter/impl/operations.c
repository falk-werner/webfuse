#include "webfuse/adapter/impl/operations.h"
#include "webfuse/adapter/impl/session_manager.h"
#include "webfuse/adapter/impl/session.h"
#include <stddef.h>

struct wf_impl_jsonrpc_server * wf_impl_operations_context_get_server(
	struct wf_impl_operations_context * context,
	fuse_ino_t inode)
{
    struct wf_impl_jsonrpc_server * server = NULL;

    struct wf_impl_session_manager * session_manger = context->session_manager;
    struct wf_impl_session * session = wf_impl_session_manager_get_by_inode(session_manger, inode);
    if (NULL != session)
    {
        server = &session->rpc;
    }

    return server;
}
