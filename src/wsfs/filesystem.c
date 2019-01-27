#include "wsfs/filesystem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>

#include "wsfs/util.h"
#include "wsfs/jsonrpc.h"

struct wsfs_filesystem
{
	struct wsfs_jsonrpc * rpc;
};

struct wsfs_filesystem * wsfs_filesystem_create(
	struct wsfs_jsonrpc * rpc)
{
	struct wsfs_filesystem * filesystem = malloc(sizeof(struct wsfs_filesystem));
	if (NULL != filesystem)
	{
		filesystem->rpc = rpc;
	}

	return filesystem;
}

void wsfs_filesystem_dispose(
	struct wsfs_filesystem * filesystem)
{
	free(filesystem);
}

static int wsfs_json_get_int(json_t * object, char const * key, int default_value)
{
	int result = default_value;

	json_t * holder = json_object_get(object, key);
	if ((NULL != holder) && (json_is_integer(holder)))
	{
		result = json_integer_value(holder);
	}

	return result;
}

wsfs_status wsfs_filesystem_getattr(
	struct wsfs_filesystem * filesystem,
	char const * path,
	struct stat * result)
{
	json_t * data = NULL;
	wsfs_status status = wsfs_jsonrpc_invoke(filesystem->rpc, &data, "getattr", "s", path);
	if (NULL != result)
	{
		json_t * mode_holder = json_object_get(data, "mode");
		json_t * type_holder = json_object_get(data, "type");
		if ((NULL != mode_holder) && (json_is_integer(mode_holder)) && 
		    (NULL != type_holder) && (json_is_string(type_holder)))
		{
			result->st_mode = json_integer_value(mode_holder) & 0777;
			char const * type = json_string_value(type_holder);
			if (0 == strcmp("file", type)) 
			{
				result->st_mode |= S_IFREG;
			}
			else if (0 == strcmp("dir", type))
			{
				result->st_mode |= S_IFDIR;
			}

			result->st_atime = wsfs_json_get_int(data, "atime", 0);
			result->st_mtime = wsfs_json_get_int(data, "mtime", 0);
			result->st_ctime = wsfs_json_get_int(data, "ctime", 0);
		}
		else
		{
			status = WSFS_BAD_PARSEERROR;
		}


		json_decref(data);
	}

	return status;
}

wsfs_status wsfs_filesystem_readdir(
	struct wsfs_filesystem * filesystem,
	char const * path,
	void * WSFS_UNUSED_PARAM(buffer),
	wsfs_add_entry_fn * WSFS_UNUSED_PARAM(add_entry))
{
	json_t * result = NULL;
	wsfs_status const status = wsfs_jsonrpc_invoke(filesystem->rpc, &result, "readdir", "s", path);
	if (NULL != result)
	{
		json_decref(result);
	}

	return status;
}

