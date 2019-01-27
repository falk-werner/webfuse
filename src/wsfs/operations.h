#ifndef _WSFS_OPERATIONS
#define _WSFS_OPERATIONS

#include "wsfs/fuse_wrapper.h"

extern void wsfs_operations_init(
	struct fuse_operations * operations);

#endif

