#ifndef WSFS_ADAPTER_IMPL_FILESYSTEM_H
#define WSFS_ADAPTER_IMPL_FILESYSTEM_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include "wsfs/adapter/impl/fuse_wrapper.h"
#include "wsfs/adapter/impl/operations.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wsfs_impl_jsonrpc_server;

struct wsfs_impl_filesystem
{
	struct fuse_args args;
	struct fuse_session * session;
	struct fuse_buf buffer;
	struct wsfs_impl_operations_context user_data;
};

extern bool wsfs_impl_filesystem_init(
    struct wsfs_impl_filesystem * filesystem,
    struct wsfs_impl_jsonrpc_server * rpc,
    char * mount_point);

extern void wsfs_impl_filesystem_cleanup(
    struct wsfs_impl_filesystem * filesystem);

extern int wsfs_impl_filesystem_get_fd(
    struct wsfs_impl_filesystem * filesystem);

extern void wsfs_impl_filesystem_process_request(
    struct wsfs_impl_filesystem * filesystem);

#ifdef __cplusplus
}
#endif


#endif
