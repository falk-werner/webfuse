#ifndef WF_PROVIDER_IMPL_CLIENT_PROTOCOL_H
#define WF_PROVIDER_IMPL_CLIENT_PROTOCOL_H

#include "webfuse/provider/impl/provider.h"
#include "webfuse/provider/impl/request.h"

#include "webfuse/core/message_queue.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_provider;
struct lws_protocols;

struct wfp_client_protocol
{
    struct wfp_request request;
    struct wfp_provider provider;
    void * user_data;
    struct lws * wsi;
    struct wf_message_queue queue;
};

extern void wfp_impl_client_protocol_init(
    struct wfp_client_protocol * protocol,
    struct wfp_provider const * provider,
    void * user_data);

extern void wfp_impl_client_protocol_cleanup(
    struct wfp_client_protocol * protocol);

extern struct wfp_client_protocol * wfp_impl_client_protocol_create(
    struct wfp_provider const * provider,
    void * user_data);

extern void wfp_impl_client_protocol_dispose(
    struct wfp_client_protocol * protocol);

extern void wfp_impl_client_protocol_init_lws(
    struct wfp_client_protocol * protocol,
    struct lws_protocols * lws_protocol);

#ifdef __cplusplus
} 
#endif

#endif
