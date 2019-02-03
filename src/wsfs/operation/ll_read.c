#include "wsfs/operations.h"
#include <errno.h>
#include "wsfs/util.h"

#include <stdio.h>

void wsfs_operation_ll_read(
	fuse_req_t request,
	fuse_ino_t WSFS_UNUSED_PARAM(inode),
    size_t WSFS_UNUSED_PARAM(size),
    off_t WSFS_UNUSED_PARAM(offset),
	struct fuse_file_info * WSFS_UNUSED_PARAM(file_info))
{
    puts("read");
   	fuse_reply_err(request, ENOENT);
}