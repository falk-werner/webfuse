#ifndef _WSFS_OPERATIONS
#define _WSFS_OPERATIONS

#include "wsfs/fuse_wrapper.h"

struct wsfs_jsonrpc;

struct wsfs_operations_context
{
	struct wsfs_jsonrpc * rpc;
	double timeout;
};

#ifdef __cplusplus
extern "C" {
#endif

extern int wsfs_operations_loop(
	char * mount_point,
	struct wsfs_jsonrpc * rpc);

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

