#include "wsfs/adapter/impl/operations.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> 

#include "wsfs/adapter/impl/jsonrpc/server.h"
#include "wsfs/util.h"


#define WSFS_DIRBUFFER_INITIAL_SIZE 1024

struct operation_readdir_context
{
	fuse_req_t request;
	size_t size;
	off_t offset;
};

struct dirbuffer
{
	char * data;
	size_t position;
	size_t capacity;
};

static void dirbuffer_init(
	struct dirbuffer * buffer)
{
	buffer->data = malloc(WSFS_DIRBUFFER_INITIAL_SIZE);
	buffer->position = 0;
	buffer->capacity = WSFS_DIRBUFFER_INITIAL_SIZE;
}

static void dirbuffer_dispose(
	struct dirbuffer * buffer)
{
	free(buffer->data);
}

static void dirbuffer_add(
	fuse_req_t request,
	struct dirbuffer * buffer,
	char const * name,
	fuse_ino_t inode)
{
	size_t const size = fuse_add_direntry(request, NULL, 0, name, NULL, 0);
	size_t remaining = buffer->capacity - buffer->position;
	while (remaining < size)
	{
		buffer->capacity *= 2;
		buffer->data = realloc(buffer->data, buffer->capacity);
		remaining = buffer->capacity - buffer->position;
	}

	struct stat stat_buffer;
	memset(&stat_buffer, 0, sizeof(struct stat));
	stat_buffer.st_ino = inode;
	fuse_add_direntry(request, 
		&buffer->data[buffer->position], remaining, name,
		&stat_buffer, buffer->position + size);
	buffer->position += size;
}

static size_t min(size_t a, size_t b)
{
	return (a < b) ? a : b;
}

static void operation_readdir_finished(
	void * user_data,
	wsfs_status status,
	json_t const * result)
{
	struct operation_readdir_context * context = user_data;

	struct dirbuffer buffer;
	dirbuffer_init(&buffer);

	if (NULL != result)
	{
		if (json_is_array(result)) 
		{
			bool buffer_full = false;
			size_t const count = json_array_size(result);
			for(size_t i = 0; (!buffer_full) && (i < count); i++)
			{
				json_t * entry =json_array_get(result, i);
				if (json_is_object(entry))
				{
					json_t * name_holder = json_object_get(entry, "name");
					json_t * inode_holder = json_object_get(entry, "inode");

					if ((NULL != name_holder) && (json_is_string(name_holder)) &&
						(NULL != inode_holder) && (json_is_integer(inode_holder)))
					{
						char const * name = json_string_value(name_holder);
						fuse_ino_t entry_inode = (fuse_ino_t) json_integer_value(inode_holder);
						dirbuffer_add(context->request, &buffer, name, entry_inode);	
					}
				}
			}
		}
	}

	if (WSFS_GOOD == status)
	{
		if (((size_t) context->offset) < buffer.position)
		{
			fuse_reply_buf(context->request, &buffer.data[context->offset],
				min(buffer.position - context->offset, context->size));
		}
		else
		{
			fuse_reply_buf(context->request, NULL, 0);			
		}
		
	}
	else
	{
		fuse_reply_err(context->request, ENOENT);
	}

	dirbuffer_dispose(&buffer);
	free(context);
}

void operation_readdir (
	fuse_req_t request,
	fuse_ino_t inode,
	size_t size,
	off_t offset,
	struct fuse_file_info * WSFS_UNUSED_PARAM(file_info))
{
    struct operations_context * user_data = fuse_req_userdata(request);
    struct jsonrpc_server * rpc = user_data->rpc;
	struct operation_readdir_context * readdir_context = malloc(sizeof(struct operation_readdir_context));
	readdir_context->request = request;
	readdir_context->size = size;
	readdir_context->offset = offset;

	jsonrpc_server_invoke(rpc, &operation_readdir_finished, readdir_context, "readdir", "i", inode);
}
