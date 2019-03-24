#ifndef WSFS_PROVIDER_CLIENT_PROTOCOL_INTERN_H
#define WSFS_PROVIDER_CLIENT_PROTOCOL_INTERN_H

#include "wsfs/provider/client_protocol.h"
#include "wsfs/provider/provider.h"
#include "wsfs/provider/request.h"

#include "wsfs/core/message_queue.h"

struct wsfsp_client_protocol
{
    struct wsfsp_request request;
    struct wsfsp_provider provider;
    void * user_data;
    struct lws * wsi;
    struct wsfs_message_queue queue;
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
