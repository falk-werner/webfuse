#ifndef WSFS_PROVIDER_CLIENT_PROTOCOL_H
#define WSFS_PROVIDER_CLIENT_PROTOCOL_H

#include "wsfs/provider/api.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wsfsp_client_protocol;
struct wsfsp_provider;
struct lws_protocols;

extern WSFSP_API struct wsfsp_client_protocol * wsfsp_client_protocol_create(
    struct wsfsp_provider const * provider,
    void * user_data);

extern WSFSP_API void wsfsp_client_protocol_dispose(
    struct wsfsp_client_protocol * protocol);

extern WSFSP_API void wsfsp_client_protocol_init_lws(
    struct wsfsp_client_protocol * protocol,
    struct lws_protocols * lws_protocol);

#ifdef __cplusplus
}
#endif



#endif
