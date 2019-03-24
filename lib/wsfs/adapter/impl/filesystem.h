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

struct jsonrpc_server;

struct filesystem
{
	struct fuse_args args;
	struct fuse_session * session;
	struct fuse_buf buffer;
	struct operations_context user_data;
};

extern bool filesystem_init(
    struct filesystem * filesystem,
    struct jsonrpc_server * rpc,
    char * mount_point);

extern void filesystem_cleanup(
    struct filesystem * filesystem);

extern int filesystem_get_fd(
    struct filesystem * filesystem);

extern void filesystem_process_request(
    struct filesystem * filesystem);

#ifdef __cplusplus
}
#endif


#endif
