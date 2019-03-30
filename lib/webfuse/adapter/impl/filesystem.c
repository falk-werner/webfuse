#include "webfuse/adapter/impl/filesystem.h"
#include "webfuse/adapter/impl/operations.h"
#include "webfuse/adapter/impl/jsonrpc/server.h"

#include <stddef.h>
#include <string.h>
#include <errno.h>

static struct fuse_lowlevel_ops const filesystem_operations =
{
	.lookup = &wf_impl_operation_lookup,
	.getattr = &wf_impl_operation_getattr,
	.readdir = &wf_impl_operation_readdir,
	.open	= &wf_impl_operation_open,
	.release = &wf_impl_operation_close,
	.read	= &wf_impl_operation_read
};


bool wf_impl_filesystem_init(
    struct wf_impl_filesystem * filesystem,
    struct wf_impl_session_manager * session_manager,
    char * mount_point)
{
	bool result = false;

	char * argv[] = {"", NULL};
	filesystem->args.argc = 1;
	filesystem->args.argv = argv;
	filesystem->args.allocated = 0;

	filesystem->user_data.session_manager = session_manager;
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

void wf_impl_filesystem_cleanup(
    struct wf_impl_filesystem * filesystem)
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

int wf_impl_filesystem_get_fd(
    struct wf_impl_filesystem * filesystem)
{
    return fuse_session_fd(filesystem->session);
}

void wf_impl_filesystem_process_request(
    struct wf_impl_filesystem * filesystem)
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
