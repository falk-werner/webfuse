#ifndef WF_ADAPTER_SERVER_H
#define WF_ADAPTER_SERVER_H

#include "webfuse/adapter/api.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_server;
struct wf_server_config;

extern WF_API struct wf_server * wf_server_create(
    struct wf_server_config * config);

extern WF_API void wf_server_dispose(
    struct wf_server * server);

extern WF_API void wf_server_run(
    struct wf_server * server);

extern WF_API void wf_server_shutdown(
    struct wf_server * server);

#ifdef __cplusplus
}
#endif


#endif
