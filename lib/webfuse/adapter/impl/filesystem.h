#ifndef WF_ADAPTER_IMPL_FILESYSTEM_H
#define WF_ADAPTER_IMPL_FILESYSTEM_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include "webfuse/adapter/impl/fuse_wrapper.h"
#include "webfuse/adapter/impl/operations.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_impl_jsonrpc_server;

struct wf_impl_filesystem
{
	struct fuse_args args;
	struct fuse_session * session;
	struct fuse_buf buffer;
	struct wf_impl_operations_context user_data;
};

extern bool wf_impl_filesystem_init(
    struct wf_impl_filesystem * filesystem,
    struct wf_impl_jsonrpc_server * rpc,
    char * mount_point);

extern void wf_impl_filesystem_cleanup(
    struct wf_impl_filesystem * filesystem);

extern int wf_impl_filesystem_get_fd(
    struct wf_impl_filesystem * filesystem);

extern void wf_impl_filesystem_process_request(
    struct wf_impl_filesystem * filesystem);

#ifdef __cplusplus
}
#endif


#endif
