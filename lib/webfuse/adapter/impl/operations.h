#ifndef WF_ADAPTER_IMPL_OPERATIONS_H
#define WF_ADAPTER_IMPL_OPERATIONS_H

#include "webfuse/adapter/impl/fuse_wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

struct wf_impl_session_manager;

struct wf_impl_operations_context
{
	struct wf_impl_session_manager * session_manager;
	double timeout;
};

extern void wf_impl_operation_lookup (
	fuse_req_t req, 
	fuse_ino_t parent, 
	char const * name);

extern void wf_impl_operation_getattr (
		fuse_req_t request,
		fuse_ino_t inode,
		struct fuse_file_info *file_info);

extern void wf_impl_operation_readdir (
	fuse_req_t request,
	fuse_ino_t inode,
	size_t size,
	off_t offset,
	struct fuse_file_info *file_info);

extern void wf_impl_operation_open(
	fuse_req_t request,
	fuse_ino_t inode,
	struct fuse_file_info * file_info);

extern void wf_impl_operation_close(
	fuse_req_t request,
	fuse_ino_t inode,
	struct fuse_file_info * file_info);

extern void wf_impl_operation_read(
	fuse_req_t request,
	fuse_ino_t ino, size_t size, off_t off,
			struct fuse_file_info *fi);

extern struct wf_impl_jsonrpc_server * wf_impl_operations_context_get_server(
	struct wf_impl_operations_context * context,
	fuse_ino_t inode);

#ifdef __cplusplus
}
#endif

#endif

