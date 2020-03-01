#include "webfuse/adapter/impl/uuid_mountpoint.h"
#include "webfuse/adapter/impl/mountpoint.h"

#include "webfuse/core/string.h"

#include <uuid/uuid.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct wf_impl_uuid_mountpoint_data
{
	char * id;
	char * filesystem_path;
	char * default_path;	
	char * full_path;	
};

static char * wf_impl_uuid_mountpoint_create_id(void)
{
    uuid_t uuid;
    uuid_generate(uuid);
    char id[UUID_STR_LEN];
    uuid_unparse(uuid, id);

	return strdup(id);
}

static bool wf_impl_uuid_mountpoint_is_link_broken(char const * path, char const * id)
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

static bool wf_impl_uuid_mountpoint_link_first_subdir(
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
				int rc = symlink(entry->d_name, link_path);
				result = (0 == rc);
				break;
			}

			entry = readdir(dir);
		}

		closedir(dir);
	}

	return result;
}

static void
wf_impl_uuid_mountpoint_data_dispose(
	void * user_data)
{
	struct wf_impl_uuid_mountpoint_data * data = user_data;

	rmdir(data->full_path);

	if (wf_impl_uuid_mountpoint_is_link_broken(data->default_path, data->id))
	{
		unlink(data->default_path);

		bool const success = wf_impl_uuid_mountpoint_link_first_subdir(data->default_path, data->filesystem_path);
		if (!success)
		{
			rmdir(data->filesystem_path);
		}
	}

	free(data->id);
	free(data->filesystem_path);
	free(data->default_path);
	free(data->full_path);
	free(data);
}

struct wf_mountpoint *
wf_impl_uuid_mountpoint_create(
    char const * root_path,
    char const * filesystem)
{
	struct wf_impl_uuid_mountpoint_data * data = malloc(sizeof(struct wf_impl_uuid_mountpoint_data));
	data->filesystem_path = wf_create_string("%s/%s", root_path, filesystem);
	mkdir(data->filesystem_path, 0755);

	data->id = wf_impl_uuid_mountpoint_create_id();
	data->full_path = wf_create_string("%s/%s/%s", root_path, filesystem, data->id);
	mkdir(data->full_path, 0755);

	data->default_path = wf_create_string("%s/%s/default", root_path, filesystem);
	int rc =  symlink(data->id, data->default_path);
	(void) rc; // ignore missing symlink

	struct wf_mountpoint * mountpoint = wf_impl_mountpoint_create(data->full_path);
	wf_impl_mountpoint_set_userdata(mountpoint, data, &wf_impl_uuid_mountpoint_data_dispose);

    return mountpoint;
}
