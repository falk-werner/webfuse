#ifndef WSFS_ADAPTER_IMPL_OPERATIONS_H
#define WSFS_ADAPTER_IMPL_OPERATIONS_H

#include "wsfs/adapter/impl/fuse_wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

struct wsfs_impl_jsonrpc_server;

struct wsfs_impl_operations_context
{
	struct wsfs_impl_jsonrpc_server * rpc;
	double timeout;
};

extern void wsfs_impl_operation_lookup (
	fuse_req_t req, 
	fuse_ino_t parent, 
	char const * name);

extern void wsfs_impl_operation_getattr (
		fuse_req_t request,
		fuse_ino_t inode,
		struct fuse_file_info *file_info);

extern void wsfs_impl_operation_readdir (
	fuse_req_t request,
	fuse_ino_t inode,
	size_t size,
	off_t offset,
	struct fuse_file_info *file_info);

extern void wsfs_impl_operation_open(
	fuse_req_t request,
	fuse_ino_t inode,
	struct fuse_file_info * file_info);

extern void wsfs_impl_operation_close(
	fuse_req_t request,
	fuse_ino_t inode,
	struct fuse_file_info * file_info);

extern void wsfs_impl_operation_read(
	fuse_req_t request,
	fuse_ino_t ino, size_t size, off_t off,
			struct fuse_file_info *fi);

#ifdef __cplusplus
}
#endif

#endif

