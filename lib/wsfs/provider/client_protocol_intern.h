#ifndef WSFSP_CLIENT_PROTOCOL_INTERN_H
#define WSFSP_CLIENT_PROTOCOL_INTERN_H

#include "wsfs/provider/client_protocol.h"
#include "wsfs/provider/provider.h"
#include "wsfs/provider/request.h"

struct wsfsp_client_protocol
{
    struct wsfsp_request request;
    struct wsfsp_provider provider;
    void * user_data;
};

#ifdef __cplusplus
extern "C"
{
#endif

extern void wsfsp_client_protocol_init(
    struct wsfsp_client_protocol * protocol,
    struct wsfsp_provider const * provider,
    void * user_data);

extern void wsfsp_client_protocol_cleanup(
    struct wsfsp_client_protocol * protocol);

#ifdef __cplusplus
} 
#endif

#endif
