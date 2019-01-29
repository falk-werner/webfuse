#include "wsfs/operations.h"

#include <string.h>

#include "wsfs/util.h"
#include "wsfs/jsonrpc.h"

static void* wsfs_operation_init(
	struct fuse_conn_info * WSFS_UNUSED_PARAM(connection),
	struct fuse_config * config)
{
	struct fuse_context * const context = fuse_get_context();
	config->kernel_cache = 1;

	return context->private_data;
}

void wsfs_operations_init(
	struct fuse_operations * operations)
{
	memset(operations, 0, sizeof(struct fuse_operations));
	operations->init    = &wsfs_operation_init;
	operations->getattr = &wsfs_operation_getattr;
	operations->readdir = &wsfs_operation_readdir;
	operations->open    = &wsfs_operation_open;
	operations->read    = &wsfs_operation_read;
}

