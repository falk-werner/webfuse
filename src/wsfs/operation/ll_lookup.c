#include "wsfs/operations.h"
#include <errno.h>
#include "wsfs/util.h"

void wsfs_operation_ll_lookup (
	fuse_req_t request, 
	fuse_ino_t WSFS_UNUSED_PARAM(parent), 
	char const * WSFS_UNUSED_PARAM(name))
{
	fuse_reply_err(request, ENOENT);
}
