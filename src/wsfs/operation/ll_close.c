#include "wsfs/operations.h"
#include <errno.h>
#include "wsfs/util.h"

void wsfs_operation_ll_close(
	fuse_req_t request,
	fuse_ino_t WSFS_UNUSED_PARAM(inode),
	struct fuse_file_info * WSFS_UNUSED_PARAM(file_info))
{
	fuse_reply_err(request, ENOENT);
}
