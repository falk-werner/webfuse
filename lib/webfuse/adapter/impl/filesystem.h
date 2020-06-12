#ifndef WF_ADAPTER_IMPL_FILESYSTEM_H
#define WF_ADAPTER_IMPL_FILESYSTEM_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include "webfuse/adapter/impl/fuse_wrapper.h"
#include "webfuse/adapter/impl/operation/context.h"
#include "webfuse/core/slist.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_mountpoint;
struct wf_jsonrpc_proxy;
struct lws;

struct wf_impl_filesystem
{
    struct wf_slist_item item;
	struct fuse_args args;
	struct fuse_session * session;
	struct fuse_buf buffer;
	struct wf_impl_operation_context user_data;
    struct lws * wsi;
    struct wf_mountpoint * mountpoint;
};

extern struct wf_impl_filesystem * wf_impl_filesystem_create(
    struct lws * session_wsi,
    struct wf_jsonrpc_proxy * proxy,
    char const * name,
    struct wf_mountpoint * mountpoint);

extern void wf_impl_filesystem_dispose(
    struct wf_impl_filesystem * filesystem);

extern void wf_impl_filesystem_process_request(
    struct wf_impl_filesystem * filesystem);

#ifdef __cplusplus
}
#endif


#endif
