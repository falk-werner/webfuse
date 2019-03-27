#ifndef WF_ADAPTER_SERVER_PROTOCOL_H
#define WF_ADAPTER_SERVER_PROTOCOL_H

#include <webfuse/adapter/api.h>
#include <webfuse/adapter/authenticate.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_server_protocol;
struct lws_protocols;

extern WF_API struct wf_server_protocol * wf_server_protocol_create(
    char * mount_point);

extern WF_API void wf_server_protocol_dispose(
    struct wf_server_protocol * protocol);

extern WF_API void wf_server_protocol_init_lws(
    struct wf_server_protocol * protocol,
    struct lws_protocols * lws_protocol);

extern WF_API void wf_server_protocol_add_authenticator(
    struct wf_server_protocol * protocol,
    char const * type,
    wf_authenticate_fn * authenticate,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif
