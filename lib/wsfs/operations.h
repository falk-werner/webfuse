#ifndef WSFS_OPERATIONS
#define WSFS_OPERATIONS

#include "wsfs/fuse_wrapper.h"

struct wsfs_jsonrpc_server;

struct wsfs_operations_context
{
	struct wsfs_jsonrpc_server * rpc;
	double timeout;
};

#ifdef __cplusplus
extern "C" {
#endif

extern void wsfs_operation_lookup (
	fuse_req_t req, 
	fuse_ino_t parent, 
	char const * name);

extern void wsfs_operation_getattr (
		fuse_req_t request,
		fuse_ino_t inode,
		struct fuse_file_info *file_info);

extern void wsfs_operation_readdir (
	fuse_req_t request,
	fuse_ino_t inode,
	size_t size,
	off_t offset,
	struct fuse_file_info *file_info);

extern void wsfs_operation_open(
	fuse_req_t request,
	fuse_ino_t inode,
	struct fuse_file_info * file_info);

extern void wsfs_operation_close(
	fuse_req_t request,
	fuse_ino_t inode,
	struct fuse_file_info * file_info);

extern void wsfs_operation_read(
	fuse_req_t request,
	fuse_ino_t ino, size_t size, off_t off,
			struct fuse_file_info *fi);

#ifdef __cplusplus
}
#endif

#endif

