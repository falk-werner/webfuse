#ifndef _WSFS_OPERATIONS
#define _WSFS_OPERATIONS

#include "wsfs/fuse_wrapper.h"

struct wsfs_jsonrpc;

#ifdef __cplusplus
extern "C" {
#endif

extern int wsfs_operations_loop(
	char * mount_point,
	struct wsfs_jsonrpc * rpc);

extern int wsfs_operation_readdir(
	char const * path,
	void * buffer,
	fuse_fill_dir_t filler,
	off_t offset,
	struct fuse_file_info * file_info,
	enum fuse_readdir_flags flags);

extern int wsfs_operation_getattr(
	char const * path,
	struct stat * buffer,
	struct fuse_file_info * file_info);

extern int wsfs_operation_open(
	char const *path,
	struct fuse_file_info * file_info);

extern int wsfs_operation_close(
	char const *path,
	struct fuse_file_info * file_info);

extern int wsfs_operation_read(
	const char * path,
 	char * buffer,
	size_t buffer_size,
	off_t  offset,
	struct fuse_file_info * file_info);

// low level operations 

extern int wsfs_operations_loop_ll(
	char * mount_point,
	struct wsfs_jsonrpc * rpc);

extern void wsfs_operation_ll_lookup (
	fuse_req_t req, 
	fuse_ino_t parent, 
	char const * name);

extern void wsfs_operation_ll_getattr (
		fuse_req_t request,
		fuse_ino_t inode,
		struct fuse_file_info *file_info);

extern void wsfs_operation_ll_readdir (
	fuse_req_t request,
	fuse_ino_t inode,
	size_t size,
	off_t offset,
	struct fuse_file_info *file_info);

extern void wsfs_operation_ll_open(
	fuse_req_t request,
	fuse_ino_t inode,
	struct fuse_file_info * file_info);

extern void wsfs_operation_ll_close(
	fuse_req_t request,
	fuse_ino_t inode,
	struct fuse_file_info * file_info);

extern void wsfs_operation_ll_read(
	fuse_req_t request,
	fuse_ino_t ino, size_t size, off_t off,
			struct fuse_file_info *fi);

#ifdef __cplusplus
}
#endif

#endif

