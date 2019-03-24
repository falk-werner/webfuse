#ifndef WSFS_ADAPTER_IMPL_SERVER_H
#define WSFS_ADAPTER_IMPL_SERVER_H

#ifdef __cplusplus
extern "C"
{
#endif

struct wsfs_server;
struct wsfs_server_config;

extern struct wsfs_server * server_create(
    struct wsfs_server_config * config);

extern void server_dispose(
    struct wsfs_server * server);

extern void server_run(
    struct wsfs_server * server);

extern void server_shutdown(
    struct wsfs_server * server);

#ifdef __cplusplus
}
#endif


#endif
