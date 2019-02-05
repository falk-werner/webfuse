#ifndef _WSFS_FILESYSTEM_H
#define _WSFS_FILESYSTEM_H

#include "wsfs/fuse_wrapper.h"
#include "wsfs/operations.h"

struct wsfs_filesystem
{
	struct fuse_args args;
	struct fuse_session * session;
	struct fuse_buf buffer;
	struct wsfs_operations_context user_data;
};

extern void wsfs_filesystem_init(
    struct wsfs_filesystem * filesystem,
    char * mount_point);

extern void wsfs_filesystem_cleanup(
    struct wsfs_filesystem * filesystem);

extern int wsfs_filesystem_get_fd(
    struct wsfs_filesystem * filesystem);

extern void wsfs_filesystem_process_request(
    struct wsfs_filesystem * filesystem);

#endif
