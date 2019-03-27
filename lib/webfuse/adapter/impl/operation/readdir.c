#include "webfuse/adapter/impl/operations.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> 

#include "webfuse/adapter/impl/jsonrpc/server.h"
#include "webfuse/core/util.h"


#define WF_DIRBUFFER_INITIAL_SIZE 1024

struct wf_impl_operation_readdir_context
{
	fuse_req_t request;
	size_t size;
	off_t offset;
};

struct wf_impl_dirbuffer
{
	char * data;
	size_t position;
	size_t capacity;
};

static void wf_impl_dirbuffer_init(
	struct wf_impl_dirbuffer * buffer)
{
	buffer->data = malloc(WF_DIRBUFFER_INITIAL_SIZE);
	buffer->position = 0;
	buffer->capacity = WF_DIRBUFFER_INITIAL_SIZE;
}

static void wf_impl_dirbuffer_dispose(
	struct wf_impl_dirbuffer * buffer)
{
	free(buffer->data);
}

static void wf_impl_dirbuffer_add(
	fuse_req_t request,
	struct wf_impl_dirbuffer * buffer,
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

static size_t wf_impl_min(size_t a, size_t b)
{
	return (a < b) ? a : b;
}

static void wf_impl_operation_readdir_finished(
	void * user_data,
	wf_status status,
	json_t const * result)
{
	struct wf_impl_operation_readdir_context * context = user_data;

	struct wf_impl_dirbuffer buffer;
	wf_impl_dirbuffer_init(&buffer);

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
						wf_impl_dirbuffer_add(context->request, &buffer, name, entry_inode);	
					}
				}
			}
		}
	}

	if (WF_GOOD == status)
	{
		if (((size_t) context->offset) < buffer.position)
		{
			fuse_reply_buf(context->request, &buffer.data[context->offset],
				wf_impl_min(buffer.position - context->offset, context->size));
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

	wf_impl_dirbuffer_dispose(&buffer);
	free(context);
}

void wf_impl_operation_readdir (
	fuse_req_t request,
	fuse_ino_t inode,
	size_t size,
	off_t offset,
	struct fuse_file_info * WF_UNUSED_PARAM(file_info))
{
    struct wf_impl_operations_context * user_data = fuse_req_userdata(request);
    struct wf_impl_jsonrpc_server * rpc = user_data->rpc;
	struct wf_impl_operation_readdir_context * readdir_context = malloc(sizeof(struct wf_impl_operation_readdir_context));
	readdir_context->request = request;
	readdir_context->size = size;
	readdir_context->offset = offset;

	wf_impl_jsonrpc_server_invoke(rpc, &wf_impl_operation_readdir_finished, readdir_context, "readdir", "i", inode);
}
