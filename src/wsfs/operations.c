#include "wsfs/operations.h"

#include <stddef.h>
#include <string.h>
#include <errno.h>

#include "wsfs/util.h"
#include "wsfs/filesystem.h"

struct wsfs_readdir_context
{
	void * buffer;
	fuse_fill_dir_t filler;
};

static int wsfs_result_from_status(wsfs_status status)
{
	switch(status)
	{
		case WSFS_GOOD: return 0;
		case WSFS_BAD_NOENTRY: return -ENOENT;
		case WSFS_BAD_NOACCESS: return -EACCES;
		default: return -ENOENT;
	}
}

static struct wsfs_filesystem * wsfs_get_filesystem(void)
{
	struct fuse_context * const context = fuse_get_context();
	struct wsfs_filesystem * const filesystem =  context->private_data;

	return filesystem;
}

static bool wsfs_add_entry(void * buffer, char const * path)
{
	struct wsfs_readdir_context * context = buffer;
	int const result = context->filler(context->buffer, path, NULL, 0, 0);
	
	return (0 == result);
}

static void* wsfs_operation_init(
	struct fuse_conn_info * WSFS_UNUSED_PARAM(connection),
	struct fuse_config * config)
{
	struct fuse_context * const context = fuse_get_context();
	config->kernel_cache = 1;

	return wsfs_filesystem_create(context->private_data);
}

static void wsfs_operation_destroy(void * private_data)
{
	struct wsfs_filesystem * const filesystem = private_data;
	wsfs_filesystem_dispose(filesystem);
}

static int wsfs_operation_getattr(
	char const * path,
	struct stat * buffer,
	struct fuse_file_info * WSFS_UNUSED_PARAM(file_info))
{
	struct fuse_context * const context = fuse_get_context();
	struct wsfs_filesystem * const filesystem = wsfs_get_filesystem();

	wsfs_status const status = wsfs_filesystem_getattr(filesystem, path, buffer); 
	if (WSFS_GOOD == status)
	{
		buffer->st_uid = context->uid;
		buffer->st_gid = context->gid;
		buffer->st_nlink = 1;
	}

	return wsfs_result_from_status(status);
}

static int wsfs_operation_readdir(
	char const * path,
	void * buffer,
	fuse_fill_dir_t filler,
	off_t WSFS_UNUSED_PARAM(offset),
	struct fuse_file_info * WSFS_UNUSED_PARAM(file_info),
	enum fuse_readdir_flags WSFS_UNUSED_PARAM(flags))
{
	struct wsfs_filesystem * filesystem = wsfs_get_filesystem();
	struct wsfs_readdir_context context =
	{
		.buffer = buffer,
		.filler = filler
	};

	wsfs_status const status = wsfs_filesystem_readdir(filesystem, path, &context, &wsfs_add_entry);
	return wsfs_result_from_status(status);
}

static int wsfs_operation_open(
	char const *path,
	struct fuse_file_info * file_info)
{
	struct wsfs_filesystem * filesystem = wsfs_get_filesystem();
	wsfs_status const status = wsfs_filesystem_open(filesystem, path, file_info->flags);

	return wsfs_result_from_status(status);
}

static int wsfs_operation_read(
	const char * path,
 	char * buffer,
	size_t buffer_size,
	off_t  offset,
	struct fuse_file_info * WSFS_UNUSED_PARAM(file_info))
{
	struct wsfs_filesystem * filesystem = wsfs_get_filesystem();

	size_t count;
	wsfs_status const status = wsfs_filesystem_read(filesystem, path, buffer, buffer_size, offset, &count);
	if (WSFS_GOOD == status)
	{
		return count;
	}
	else
	{
		return wsfs_result_from_status(status);
	}

}

void wsfs_operations_init(
	struct fuse_operations * operations)
{
	memset(operations, 0, sizeof(struct fuse_operations));
	operations->init    = &wsfs_operation_init;
	operations->destroy = &wsfs_operation_destroy;
	operations->getattr = &wsfs_operation_getattr;
	operations->readdir = &wsfs_operation_readdir;
	operations->open    = &wsfs_operation_open;
	operations->read    = &wsfs_operation_read;
}

