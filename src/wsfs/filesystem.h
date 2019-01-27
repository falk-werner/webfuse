#ifndef _WSFS_FILESYSTEM_H
#define _WSFS_FILESYSTEM_H

#include <stdbool.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "wsfs/status.h"


struct wsfs_filesystem;
struct wsfs_jsonrpc;

typedef bool wsfs_add_entry_fn(void * buffer, char const * path);

extern struct wsfs_filesystem * wsfs_filesystem_create(
	struct wsfs_jsonrpc * service);

extern void wsfs_filesystem_dispose(
	struct wsfs_filesystem * filesystem);

extern wsfs_status wsfs_filesystem_getattr(
	struct wsfs_filesystem * filesystem,
	char const * path,
	struct stat * result);

extern wsfs_status wsfs_filesystem_readdir(
	struct wsfs_filesystem * filesystem,
	char const * path,
	void * buffer,
	wsfs_add_entry_fn * add_entry);

extern wsfs_status wsfs_filesystem_open(
	struct wsfs_filesystem * filesystem,
	char const * path,
	int flags);

extern wsfs_status wsfs_filesystem_read(
	struct wsfs_filesystem * filesystem,
	char const * path,
	char * buffer,
	size_t buffer_size,
	size_t offset,
	size_t * read_count);

#endif

