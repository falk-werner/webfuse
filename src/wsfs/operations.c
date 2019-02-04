#include "wsfs/operations.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <uv.h>

#include "wsfs/util.h"

struct wsfs_fuse_context
{
	struct fuse_args args;
	struct fuse_session * session;
	struct fuse_buf buffer;
	struct wsfs_operations_context user_data;
	int fd;
	bool mounted;
};

static struct fuse_lowlevel_ops const wsfs_operations =
{
	.lookup = &wsfs_operation_lookup,
	.getattr = &wsfs_operation_getattr,
	.readdir = &wsfs_operation_readdir,
	.open	= &wsfs_operation_open,
	.release = &wsfs_operation_close,
	.read	= &wsfs_operation_read
};


static bool wsfs_fuse_init(
	struct wsfs_fuse_context * context,
	char * mount_point,
	struct wsfs_jsonrpc * rpc)
{
	bool result = false;

	char * argv[] = {"", NULL};
	context->args.argc = 1;
	context->args.argv = argv;
	context->args.allocated = 0;

	context->user_data.rpc = rpc;
	context->user_data.timeout = 1.0;
	memset(&context->buffer, 0, sizeof(struct fuse_buf));

	context->session = fuse_session_new(&context->args, &wsfs_operations, sizeof(wsfs_operations), &context->user_data);
	if (NULL != context->session)
	{
		result = (0 == fuse_session_mount(context->session, mount_point));
		context->fd = fuse_session_fd(context->session);		
	}

	// cleanup on error
	if (!result)
	{
		if (NULL != context->session)
		{
			fuse_session_destroy(context->session);
			context->session = NULL;
		}

		free(context->buffer.mem);
		fuse_opt_free_args(&context->args);
	}

	return result;
}

static void wsfs_fuse_cleanup(
	struct wsfs_fuse_context * context)
{
	if (NULL != context->session)
	{
		fuse_session_reset(context->session);
		fuse_session_unmount(context->session);
		fuse_session_destroy(context->session);
		context->session = NULL;
	}

	free(context->buffer.mem);
	fuse_opt_free_args(&context->args);
}

static void wsfs_fuse_onreadable(
	uv_poll_t* handle,
	int WSFS_UNUSED_PARAM(status),
	int WSFS_UNUSED_PARAM(events))
{
	struct wsfs_fuse_context * context = handle->data;
	int const result = fuse_session_receive_buf(context->session, &context->buffer);
	if (0 < result)
	{
		fuse_session_process_buf(context->session, &context->buffer);
	}
	else if (-EINTR != result)
	{
		uv_poll_stop(handle);
	}
}

static void wsfs_fuse_oninterrupt(uv_signal_t* handle, int WSFS_UNUSED_PARAM(signum))
{
	uv_poll_t * poll_handle = handle->data;
	uv_poll_stop(poll_handle);
}

int wsfs_operations_loop(
	char * mount_point,
	struct wsfs_jsonrpc * rpc)
{
	uv_loop_t * loop = malloc(sizeof(uv_loop_t));
	uv_loop_init(loop);

	struct wsfs_fuse_context context;
	if (wsfs_fuse_init(&context, mount_point, rpc))
	{
		uv_poll_t handle;
		uv_poll_init(loop, &handle, context.fd);
		handle.data = &context;
		uv_poll_start(&handle, UV_READABLE, &wsfs_fuse_onreadable);

		uv_signal_t signal_interrupt;
		uv_signal_init(loop, &signal_interrupt);
		signal_interrupt.data = &handle;
		uv_signal_start_oneshot(&signal_interrupt, &wsfs_fuse_oninterrupt, SIGINT);

		uv_run(loop, UV_RUN_DEFAULT);

		uv_close((uv_handle_t*) &signal_interrupt, NULL);
		uv_close((uv_handle_t*) &handle, NULL);
		uv_run(loop, UV_RUN_DEFAULT);

		wsfs_fuse_cleanup(&context);
	}

	uv_loop_close(loop);
	free(loop);

	return 0;
}
