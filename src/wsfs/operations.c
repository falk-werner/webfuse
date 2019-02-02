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

static void wsfs_operations_init(
	struct fuse_operations * operations)
{
	memset(operations, 0, sizeof(struct fuse_operations));
	operations->init    = &wsfs_operation_init;
	operations->getattr = &wsfs_operation_getattr;
	operations->readdir = &wsfs_operation_readdir;
	operations->open    = &wsfs_operation_open;
	operations->release = &wsfs_operation_close;
	operations->read    = &wsfs_operation_read;
}

int wsfs_operations_loop(
	char * mount_point,
	struct wsfs_jsonrpc * rpc)
{
	struct fuse_operations operations;
	wsfs_operations_init(&operations);

	char * fuse_args[] = { "app", "-s", "-f", mount_point, NULL };
	int const result = fuse_main(4, fuse_args, &operations, rpc);

	return result;
}

int wsfs_operations_loop_ll(
	char * mount_point,
	struct wsfs_jsonrpc * rpc)
{
	struct fuse_lowlevel_ops operations;
	memset(&operations, 0, sizeof(struct fuse_lowlevel_ops));
	operations.lookup = &wsfs_operation_ll_lookup;
	operations.getattr = &wsfs_operation_ll_getattr;
	operations.readdir = &wsfs_operation_ll_readdir;
	operations.open	= &wsfs_operation_ll_open;
	operations.release = &wsfs_operation_ll_close;
	operations.read	= &wsfs_operation_ll_read;

	int result = 1;
	const int argc = 1;
	char * argv[] = {"", NULL};
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	struct fuse_session * session = fuse_session_new(&args, &operations, sizeof(operations), rpc);
	if (NULL != session)
	{
		fuse_set_signal_handlers(session);
		result = fuse_session_mount(session, mount_point);
		if (0 == result)
		{
			result = fuse_session_loop(session);
			fuse_session_unmount(session);
		}
		fuse_remove_signal_handlers(session);
		fuse_session_destroy(session);
	}

	fuse_opt_free_args(&args);
	return 0;
}
