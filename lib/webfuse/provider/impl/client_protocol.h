#ifndef WF_PROVIDER_IMPL_CLIENT_PROTOCOL_H
#define WF_PROVIDER_IMPL_CLIENT_PROTOCOL_H

#include "webfuse/provider/impl/provider.h"
#include "webfuse/provider/impl/request.h"

#include "webfuse/core/slist.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define WFP_CLIENT_PROTOCOL_NAME ("fs")

struct wfp_client_config;
struct lws_protocols;
struct lws_context;

struct wfp_client_protocol
{
    bool is_connected;
    struct wfp_request request;
    struct wfp_provider provider;
    void * user_data;
    struct lws * wsi;
    struct wf_slist messages;
};

extern void wfp_impl_client_protocol_init(
    struct wfp_client_protocol * protocol,
    struct wfp_provider const * provider,
    void * user_data);

extern void wfp_impl_client_protocol_cleanup(
    struct wfp_client_protocol * protocol);

extern struct wfp_client_protocol * wfp_impl_client_protocol_create(
    struct wfp_client_config const * config);

extern void wfp_impl_client_protocol_dispose(
    struct wfp_client_protocol * protocol);

extern void wfp_impl_client_protocol_init_lws(
    struct wfp_client_protocol * protocol,
    struct lws_protocols * lws_protocol);

extern void wfp_impl_client_protocol_connect(
    struct wfp_client_protocol * protocol,
    struct lws_context * context,
    char const * url);

#ifdef __cplusplus
} 
#endif

#endif
