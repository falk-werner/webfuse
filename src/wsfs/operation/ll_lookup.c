#include "wsfs/operations.h"
#include <errno.h>
#include "wsfs/util.h"

#include <stdio.h>

void wsfs_operation_ll_lookup (
	fuse_req_t request, 
	fuse_ino_t parent, 
	char const * name)
{

    printf("lookup: inode=%lu, name=%s\n", parent, name);
	fuse_reply_err(request, ENOENT);
}
