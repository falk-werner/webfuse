#ifndef WF_ADAPTER_IMPL_FILESYSTEM_H
#define WF_ADAPTER_IMPL_FILESYSTEM_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include "webfuse/adapter/impl/fuse_wrapper.h"
#include "webfuse/adapter/impl/operations.h"
#include "webfuse/core/dlist.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_impl_session;
struct lws;

struct wf_impl_filesystem
{
    struct wf_dlist_item item;
	struct fuse_args args;
	struct fuse_session * session;
	struct fuse_buf buffer;
	struct wf_impl_operations_context user_data;
    struct lws * wsi;
    char * name;
    char * id;
    char * service_path;
    char * default_path;
    char * root_path;
};

extern struct wf_impl_filesystem * wf_impl_filesystem_create(
    struct wf_impl_session * session,
    char const * name);

extern void wf_impl_filesystem_dispose(
    struct wf_impl_filesystem * filesystem);

extern void wf_impl_filesystem_process_request(
    struct wf_impl_filesystem * filesystem);

#ifdef __cplusplus
}
#endif


#endif
