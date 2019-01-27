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

			result->st_size = wsfs_json_get_int(data, "size", 0);
			result->st_atime = wsfs_json_get_int(data, "atime", 0);
			result->st_mtime = wsfs_json_get_int(data, "mtime", 0);
			result->st_ctime = wsfs_json_get_int(data, "ctime", 0);
		}
		else
		{
			status = WSFS_BAD_FORMAT;
		}


		json_decref(data);
	}

	return status;
}

wsfs_status wsfs_filesystem_readdir(
	struct wsfs_filesystem * filesystem,
	char const * path,
	void * buffer,
	wsfs_add_entry_fn * add_entry)
{
	json_t * result = NULL;
	wsfs_status const status = wsfs_jsonrpc_invoke(filesystem->rpc, &result, "readdir", "s", path);
	if (NULL != result)
	{
		if (json_is_array(result)) 
		{
			bool has_capacity = true;
			size_t const count = json_array_size(result);
			for(size_t i = 0; (has_capacity) && (i < count); i++)
			{
				json_t * entry =json_array_get(result, i);
				if (json_is_string(entry))
				{
					has_capacity = add_entry(buffer, json_string_value(entry));
				}
			}
		}

		json_decref(result);
	}

	return status;
}

wsfs_status wsfs_filesystem_open(
	struct wsfs_filesystem * filesystem,
	char const * path,
	int flags)
{
	json_t * result = NULL;
	wsfs_status const status = wsfs_jsonrpc_invoke(filesystem->rpc, &result, "open", "si", path, flags);
	if (NULL != result)
	{
		// unused
		json_decref(result);
	}

	return status;
}

#define WSFS_MAX_READ_LENGTH 4096

static wsfs_status wsfs_fill_buffer(
	char * buffer,
	size_t buffer_size,
	char const * format,
	char const * data,
	size_t count)
{
	wsfs_status status = WSFS_GOOD;

	size_t const copy_count = (buffer_size < count) ? buffer_size : count;
	if (0 < copy_count)
	{
		if (0 == strcmp("identity", format))
		{
			memcpy(buffer, data, copy_count);
		}
		else
		{
			status = WSFS_BAD;
		}
	}

	return status;
}

wsfs_status wsfs_filesystem_read(
	struct wsfs_filesystem * filesystem,
	char const * path,
	char * buffer,
	size_t buffer_size,
	size_t offset,
	size_t * read_count)
{
	int const length = (buffer_size <= WSFS_MAX_READ_LENGTH) ? (int) buffer_size : WSFS_MAX_READ_LENGTH;
	*read_count = 0;

	json_t * result = NULL;
	wsfs_status status = wsfs_jsonrpc_invoke(filesystem->rpc, &result, "read", "sii", path, (int) offset, length);
	if (NULL != result)
	{
		json_t * data_holder = json_object_get(result, "data");
		json_t * format_holder = json_object_get(result, "format");
		json_t * count_holder = json_object_get(result, "count");

		if ((NULL != data_holder) && (json_is_string(data_holder)) &&
             	    (NULL != format_holder) && (json_is_string(format_holder)) &&
             	    (NULL != count_holder) && (json_is_integer(count_holder)))
		{
			char const * const data = json_string_value(data_holder);
			char const * const format = json_string_value(format_holder);
			size_t const count = (size_t) json_integer_value(count_holder);

			status = wsfs_fill_buffer(buffer, buffer_size, format, data, count);
			if (WSFS_GOOD == status)
			{
				*read_count = count;
			}
		}
		else
		{
			status = WSFS_BAD_FORMAT;
		}

		json_decref(result);
	}

	return status;
}

