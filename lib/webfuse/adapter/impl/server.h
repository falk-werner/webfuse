#ifndef WF_ADAPTER_IMPL_SERVER_H
#define WF_ADAPTER_IMPL_SERVER_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_server;
struct wf_server_config;

extern struct wf_server * wf_impl_server_create(
    struct wf_server_config * config);

extern void wf_impl_server_dispose(
    struct wf_server * server);

extern bool wf_impl_server_is_operational(
    struct wf_server * server);

extern void wf_impl_server_service(
    struct wf_server * server);

extern void wf_impl_server_interrupt(
    struct wf_server * server);

extern int wf_impl_server_get_port(
    struct wf_server const * server);

#ifdef __cplusplus
}
#endif


#endif
