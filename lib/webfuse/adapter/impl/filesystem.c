#include "webfuse/adapter/impl/filesystem.h"
#include "webfuse/adapter/impl/operation/context.h"
#include "webfuse/adapter/impl/operation/open.h"
#include "webfuse/adapter/impl/operation/close.h"
#include "webfuse/adapter/impl/operation/read.h"
#include "webfuse/adapter/impl/operation/readdir.h"
#include "webfuse/adapter/impl/operation/getattr.h"
#include "webfuse/adapter/impl/operation/lookup.h"
#include "webfuse/adapter/impl/session.h"
#include "webfuse/adapter/impl/mountpoint.h"

#include <libwebsockets.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

#include <stdlib.h>
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

static void wf_impl_filesystem_cleanup(
    struct wf_impl_filesystem * filesystem)
{
	fuse_session_reset(filesystem->session);
	fuse_session_unmount(filesystem->session);
	fuse_session_destroy(filesystem->session);
	filesystem->session = NULL;		

	free(filesystem->buffer.mem);
	fuse_opt_free_args(&filesystem->args);    

	wf_mountpoint_dispose(filesystem->mountpoint);

	free(filesystem->user_data.name);
}


static bool wf_impl_filesystem_init(
    struct wf_impl_filesystem * filesystem,
    struct lws * session_wsi,
    struct wf_jsonrpc_proxy * proxy,
	char const * name,
	struct wf_mountpoint * mountpoint)
{
	bool result = false;
	
	char * argv[] = {"", NULL};
	filesystem->args.argc = 1;
	filesystem->args.argv = argv;
	filesystem->args.allocated = 0;

	filesystem->user_data.proxy = proxy;
	filesystem->user_data.timeout = 1.0;
	filesystem->user_data.name = strdup(name);
	memset(&filesystem->buffer, 0, sizeof(struct fuse_buf));

	filesystem->mountpoint = mountpoint;

	filesystem->session = fuse_session_new(
        &filesystem->args,
        &filesystem_operations,
        sizeof(filesystem_operations),
        &filesystem->user_data);
	if (NULL != filesystem->session)
	{
		char const * path = wf_mountpoint_get_path(filesystem->mountpoint);
		result = (0 == fuse_session_mount(filesystem->session, path));
	}

	if (result)
	{
        lws_sock_file_fd_type fd;
        fd.filefd = fuse_session_fd(filesystem->session);
		struct lws_protocols const * protocol = lws_get_protocol(session_wsi);
        filesystem->wsi = lws_adopt_descriptor_vhost(lws_get_vhost(session_wsi), LWS_ADOPT_RAW_FILE_DESC, fd, protocol->name, session_wsi);

		if (NULL == filesystem->wsi)
		{
			wf_impl_filesystem_cleanup(filesystem);
			result = false; 
		}

	}

	return result;
}

struct wf_impl_filesystem * wf_impl_filesystem_create(
    struct lws * session_wsi,
    struct wf_jsonrpc_proxy * proxy,
	char const * name,
	struct wf_mountpoint * mountpoint)
{
	struct wf_impl_filesystem * filesystem = malloc(sizeof(struct wf_impl_filesystem));
	bool success = wf_impl_filesystem_init(filesystem, session_wsi, proxy, name, mountpoint);
	if (!success)
	{
		free(filesystem);
		filesystem = NULL;
	}

	return filesystem;
}

void wf_impl_filesystem_dispose(
    struct wf_impl_filesystem * filesystem)
{
	wf_impl_filesystem_cleanup(filesystem);
	free(filesystem);
}

void wf_impl_filesystem_process_request(
    struct wf_impl_filesystem * filesystem)
{
	int const result = fuse_session_receive_buf(filesystem->session, &filesystem->buffer);
	if (0 < result)
	{
		fuse_session_process_buf(filesystem->session, &filesystem->buffer);
	}
}
