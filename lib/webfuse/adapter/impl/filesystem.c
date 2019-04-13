#include "webfuse/adapter/impl/filesystem.h"
#include "webfuse/adapter/impl/operations.h"
#include "webfuse/adapter/impl/session.h"

#include <libwebsockets.h>
#include <uuid/uuid.h>

#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>

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

static char * wf_impl_filesystem_create_id(void)
{
    uuid_t uuid;
    uuid_generate(uuid);
    char id[UUID_STR_LEN];
    uuid_unparse(uuid, id);

	return strdup(id);
}

static bool wf_impl_filesystem_is_link_broken(char const * path)
{
	char buffer[PATH_MAX];
	ssize_t count = readlink(path, buffer, PATH_MAX);
	return (0 < count);
}

static void wf_impl_filesystem_cleanup(
    struct wf_impl_filesystem * filesystem)
{
	fuse_session_reset(filesystem->session);
	fuse_session_unmount(filesystem->session);
	fuse_session_destroy(filesystem->session);
	filesystem->session = NULL;		

	free(filesystem->buffer.mem);
	fuse_opt_free_args(&filesystem->args);    

	struct wf_impl_session * session = filesystem->user_data.session;
	char path[PATH_MAX];
	snprintf(path, PATH_MAX, "%s/%s/%s", session->mount_point, filesystem->user_data.name, filesystem->id);
	rmdir(path);

	snprintf(path, PATH_MAX, "%s/%s/default", session->mount_point, filesystem->user_data.name);
	rmdir(path);
	if (wf_impl_filesystem_is_link_broken(path))
	{
		unlink(path);
		// ToDo: recreate link, if any directory exists
	}

	snprintf(path, PATH_MAX, "%s/%s", session->mount_point, filesystem->user_data.name);
	rmdir(path);

	free(filesystem->user_data.name);
	free(filesystem->id);
}


static bool wf_impl_filesystem_init(
    struct wf_impl_filesystem * filesystem,
    struct wf_impl_session * session,
	char const * name)
{
	bool result = false;
	wf_dlist_item_init(&filesystem->item);

	char * argv[] = {"", NULL};
	filesystem->args.argc = 1;
	filesystem->args.argv = argv;
	filesystem->args.allocated = 0;

	filesystem->user_data.session = session;
	filesystem->user_data.timeout = 1.0;
	filesystem->user_data.name = strdup(name);
	filesystem->id = wf_impl_filesystem_create_id();
	memset(&filesystem->buffer, 0, sizeof(struct fuse_buf));

	char path[PATH_MAX];
	snprintf(path, PATH_MAX, "%s/%s", session->mount_point, name);
	mkdir(path, 0755);

	snprintf(path, PATH_MAX, "%s/%s/%s", session->mount_point, name, filesystem->id);
	mkdir(path, 0755);

	char defaultPath[PATH_MAX];
	snprintf(defaultPath, PATH_MAX, "%s/%s/default", session->mount_point, name);
	symlink(filesystem->id, defaultPath);

	filesystem->session = fuse_session_new(
        &filesystem->args,
        &filesystem_operations,
        sizeof(filesystem_operations),
        &filesystem->user_data);
	if (NULL != filesystem->session)
	{
		result = (0 == fuse_session_mount(filesystem->session, path));
	}

	if (result)
	{
        lws_sock_file_fd_type fd;
        fd.filefd = fuse_session_fd(filesystem->session);
		struct lws_protocols const * protocol = lws_get_protocol(session->wsi);
        filesystem->wsi = lws_adopt_descriptor_vhost(lws_get_vhost(session->wsi), LWS_ADOPT_RAW_FILE_DESC, fd, protocol->name, session->wsi);

		if (NULL == filesystem->wsi)
		{
			wf_impl_filesystem_cleanup(filesystem);
			result = false; 
		}

	}

	return result;
}

struct wf_impl_filesystem * wf_impl_filesystem_create(
    struct wf_impl_session * session,
	char const * name)
{
	struct wf_impl_filesystem * filesystem = malloc(sizeof(struct wf_impl_filesystem));
	if (NULL != filesystem)
	{
		bool success = wf_impl_filesystem_init(filesystem, session, name);
		if (!success)
		{
			free(filesystem);
			filesystem = NULL;
		}
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
	else if (-EINTR != result)
	{
        // ToDo
	}

}
