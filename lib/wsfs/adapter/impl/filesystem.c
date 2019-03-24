#include "wsfs/adapter/impl/filesystem.h"
#include "wsfs/adapter/impl/operations.h"
#include "wsfs/adapter/impl/jsonrpc/server.h"

#include <stddef.h>
#include <string.h>
#include <errno.h>

static struct fuse_lowlevel_ops const filesystem_operations =
{
	.lookup = &operation_lookup,
	.getattr = &operation_getattr,
	.readdir = &operation_readdir,
	.open	= &operation_open,
	.release = &operation_close,
	.read	= &operation_read
};


bool filesystem_init(
    struct filesystem * filesystem,
	struct jsonrpc_server * rpc,
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

void filesystem_cleanup(
    struct filesystem * filesystem)
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

int filesystem_get_fd(
    struct filesystem * filesystem)
{
    return fuse_session_fd(filesystem->session);
}

void filesystem_process_request(
    struct filesystem * filesystem)
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
