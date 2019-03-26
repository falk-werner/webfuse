#ifndef WSFS_PROVIDER_IMPL_CLIENT_PROTOCOL_H
#define WSFS_PROVIDER_IMPL_CLIENT_PROTOCOL_H

#include "wsfs/provider/impl/provider.h"
#include "wsfs/provider/impl/request.h"

#include "wsfs/core/message_queue.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wsfsp_provider;
struct lws_protocols;

struct wsfsp_client_protocol
{
    struct wsfsp_request request;
    struct wsfsp_provider provider;
    void * user_data;
    struct lws * wsi;
    struct wsfs_message_queue queue;
};

extern void wsfsp_impl_client_protocol_init(
    struct wsfsp_client_protocol * protocol,
    struct wsfsp_provider const * provider,
    void * user_data);

extern void wsfsp_impl_client_protocol_cleanup(
    struct wsfsp_client_protocol * protocol);

extern struct wsfsp_client_protocol * wsfsp_impl_client_protocol_create(
    struct wsfsp_provider const * provider,
    void * user_data);

extern void wsfsp_impl_client_protocol_dispose(
    struct wsfsp_client_protocol * protocol);

extern void wsfsp_impl_client_protocol_init_lws(
    struct wsfsp_client_protocol * protocol,
    struct lws_protocols * lws_protocol);

#ifdef __cplusplus
} 
#endif

#endif
