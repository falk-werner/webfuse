#include "webfuse/adapter/impl/filesystem.h"
#include "webfuse/adapter/impl/operations.h"
#include "webfuse/adapter/impl/session.h"

#include "webfuse/core/string.h"

#include <libwebsockets.h>
#include <uuid/uuid.h>

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

static char * wf_impl_filesystem_create_id(void)
{
    uuid_t uuid;
    uuid_generate(uuid);
    char id[UUID_STR_LEN];
    uuid_unparse(uuid, id);

	return strdup(id);
}

static bool wf_impl_filesystem_is_link_broken(char const * path, char const * id)
{
	bool result = false;

	char buffer[UUID_STR_LEN];
	ssize_t count = readlink(path, buffer, UUID_STR_LEN);
	if ((0 < count) && (count < UUID_STR_LEN))
	{
		buffer[count] = '\0';
		result = (0 == strcmp(buffer, id));
	}

	return result;
}

static bool wf_impl_filesystem_link_first_subdir(
	char const * link_path,
	char const * path)
{
	bool result = false;
	DIR * dir = opendir(path);
	if (NULL != dir)
	{
		struct dirent * entry = readdir(dir);
		while (NULL != entry)
		{
			if ((DT_DIR == entry->d_type) && ('.' != entry->d_name[0]))
			{
				symlink(entry->d_name, link_path);
				result = true;
				break;
			}

			entry = readdir(dir);
		}

		closedir(dir);
	}

	return result;
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

	rmdir(filesystem->root_path);

	if (wf_impl_filesystem_is_link_broken(filesystem->default_path, filesystem->id))
	{
		unlink(filesystem->default_path);

		bool const success = wf_impl_filesystem_link_first_subdir(filesystem->default_path, filesystem->service_path);
		if (!success)
		{
			rmdir(filesystem->service_path);
		}
	}


	free(filesystem->user_data.name);
	free(filesystem->id);
	free(filesystem->root_path);
	free(filesystem->default_path);
	free(filesystem->service_path);
}


static bool wf_impl_filesystem_init(
    struct wf_impl_filesystem * filesystem,
    struct wf_impl_session * session,
	char const * name)
{
	bool result = false;
	
	char * argv[] = {"", NULL};
	filesystem->args.argc = 1;
	filesystem->args.argv = argv;
	filesystem->args.allocated = 0;

	filesystem->user_data.session = session;
	filesystem->user_data.timeout = 1.0;
	filesystem->user_data.name = strdup(name);
	memset(&filesystem->buffer, 0, sizeof(struct fuse_buf));

	filesystem->service_path = wf_create_string("%s/%s", session->mount_point, name);
	mkdir(filesystem->service_path, 0755);

	filesystem->id = wf_impl_filesystem_create_id();
	filesystem->root_path =  wf_create_string("%s/%s/%s", session->mount_point, name, filesystem->id);
	mkdir(filesystem->root_path, 0755);

	filesystem->default_path =  wf_create_string("%s/%s/default", session->mount_point, name);
	symlink(filesystem->id, filesystem->default_path);

	filesystem->session = fuse_session_new(
        &filesystem->args,
        &filesystem_operations,
        sizeof(filesystem_operations),
        &filesystem->user_data);
	if (NULL != filesystem->session)
	{
		result = (0 == fuse_session_mount(filesystem->session, filesystem->root_path));
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
