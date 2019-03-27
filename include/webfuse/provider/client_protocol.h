#ifndef WF_PROVIDER_CLIENT_PROTOCOL_H
#define WF_PROVIDER_CLIENT_PROTOCOL_H

#include "webfuse/provider/api.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_client_protocol;
struct wfp_provider;
struct lws_protocols;

extern WFP_API struct wfp_client_protocol * wfp_client_protocol_create(
    struct wfp_provider const * provider,
    void * user_data);

extern WFP_API void wfp_client_protocol_dispose(
    struct wfp_client_protocol * protocol);

extern WFP_API void wfp_client_protocol_init_lws(
    struct wfp_client_protocol * protocol,
    struct lws_protocols * lws_protocol);

#ifdef __cplusplus
}
#endif



#endif
