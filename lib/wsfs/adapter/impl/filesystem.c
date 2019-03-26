#include "wsfs/adapter/impl/filesystem.h"
#include "wsfs/adapter/impl/operations.h"
#include "wsfs/adapter/impl/jsonrpc/server.h"

#include <stddef.h>
#include <string.h>
#include <errno.h>

static struct fuse_lowlevel_ops const filesystem_operations =
{
	.lookup = &wsfs_impl_operation_lookup,
	.getattr = &wsfs_impl_operation_getattr,
	.readdir = &wsfs_impl_operation_readdir,
	.open	= &wsfs_impl_operation_open,
	.release = &wsfs_impl_operation_close,
	.read	= &wsfs_impl_operation_read
};


bool wsfs_impl_filesystem_init(
    struct wsfs_impl_filesystem * filesystem,
	struct wsfs_impl_jsonrpc_server * rpc,
    char * mount_point)
{
	bool result = false;

	char * argv[] = {"", NULL};
	filesystem->args.argc = 1;
	filesystem->args.argv = argv;
	filesystem->args.allocated = 0;

	filesystem->user_data.rpc = rpc;
	filesystem->user_data.timeout = 1.0;
	memset(&filesystem->buffer, 0, sizeof(struct fuse_buf));

	filesystem->session = fuse_session_new(
        &filesystem->args,
        &filesystem_operations,
        sizeof(filesystem_operations),
        &filesystem->user_data);
	if (NULL != filesystem->session)
	{
		result = (0 == fuse_session_mount(filesystem->session, mount_point));
	}

	return result;
}

void wsfs_impl_filesystem_cleanup(
    struct wsfs_impl_filesystem * filesystem)
{
	if (NULL != filesystem->session)
	{
		fuse_session_reset(filesystem->session);
		fuse_session_unmount(filesystem->session);
		fuse_session_destroy(filesystem->session);
		filesystem->session = NULL;
	}

	free(filesystem->buffer.mem);
	fuse_opt_free_args(&filesystem->args);    
}

int wsfs_impl_filesystem_get_fd(
    struct wsfs_impl_filesystem * filesystem)
{
    return fuse_session_fd(filesystem->session);
}

void wsfs_impl_filesystem_process_request(
    struct wsfs_impl_filesystem * filesystem)
{
	int const result = fuse_session_receive_buf(filesystem->session, &filesystem->buffer);
	if (0 < result)
	{
		fuse_session_process_buf(filesystem->session, &filesystem->buffer);
	}
	else if (-EINTR != result)
	{
        // ToDo
	}

}
