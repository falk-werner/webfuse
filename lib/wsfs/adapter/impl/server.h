#ifndef WSFS_ADAPTER_IMPL_SERVER_H
#define WSFS_ADAPTER_IMPL_SERVER_H

#ifdef __cplusplus
extern "C"
{
#endif

struct server;
struct server_config;

extern struct server * server_create(
    struct server_config * config);

extern void server_dispose(
    struct server * server);

extern void server_run(
    struct server * server);

extern void server_shutdown(
    struct server * server);

#ifdef __cplusplus
}
#endif


#endif
