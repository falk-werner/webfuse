#ifndef WSFS_ADAPTER_IMPL_FILESYSTEM_H
#define WSFS_ADAPTER_IMPL_FILESYSTEM_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include "wsfs/adapter/impl/fuse_wrapper.h"
#include "wsfs/adapter/impl/operations.h"

struct wsfs_jsonrpc_server;

struct wsfs_filesystem
{
	struct fuse_args args;
	struct fuse_session * session;
	struct fuse_buf buffer;
	struct wsfs_operations_context user_data;
};

#ifdef __cplusplus
extern "C"
{
#endif

extern bool wsfs_filesystem_init(
    struct wsfs_filesystem * filesystem,
    struct wsfs_jsonrpc_server * rpc,
    char * mount_point);

extern void wsfs_filesystem_cleanup(
    struct wsfs_filesystem * filesystem);

extern int wsfs_filesystem_get_fd(
    struct wsfs_filesystem * filesystem);

extern void wsfs_filesystem_process_request(
    struct wsfs_filesystem * filesystem);

#ifdef __cplusplus
}
#endif


#endif
