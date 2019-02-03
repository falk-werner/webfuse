#include "wsfs/operations.h"

#include <string.h>

int wsfs_operations_loop(
	char * mount_point,
	struct wsfs_jsonrpc * rpc)
{
	struct fuse_lowlevel_ops operations;
	memset(&operations, 0, sizeof(struct fuse_lowlevel_ops));
	operations.lookup = &wsfs_operation_lookup;
	operations.getattr = &wsfs_operation_getattr;
	operations.readdir = &wsfs_operation_readdir;
	operations.open	= &wsfs_operation_open;
	operations.release = &wsfs_operation_close;
	operations.read	= &wsfs_operation_read;

	int result = 1;
	const int argc = 1;
	char * argv[] = {"", NULL};
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	struct wsfs_operations_context context = {
		.rpc = rpc,
		.timeout = 1.0
	};
	struct fuse_session * session = fuse_session_new(&args, &operations, sizeof(operations), &context);
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
