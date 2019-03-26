#ifndef WF_PROVIDER_CLIENT_H
#define WF_PROVIDER_CLIENT_H

#include "webfuse/provider/api.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_client;
struct wfp_client_config;

extern WFP_API struct wfp_client * wfp_client_create(
    struct wfp_client_config * config);

extern WFP_API void wfp_client_connect(
    struct wfp_client * client,
    char const * url);

extern WFP_API void wfp_client_disconnect(
    struct wfp_client * client);

extern WFP_API void wfp_client_dispose(
    struct wfp_client * client);

extern WFP_API void wfp_client_run(
    struct wfp_client * client);

extern WFP_API void wfp_client_shutdown(
    struct wfp_client * client);


#ifdef __cplusplus
}
#endif


#endif
