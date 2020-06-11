////////////////////////////////////////////////////////////////////////////////
/// \file adapter/client.h
/// \brief Adapter client.
////////////////////////////////////////////////////////////////////////////////

#ifndef WF_ADAPTER_CLIENT_H
#define WF_ADAPTER_CLIENT_H

#include <webfuse/adapter/api.h>
#include <webfuse/adapter/client_callback.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_client;

extern WF_API struct wf_client *
wf_client_create(
    wf_client_callback_fn * callback,
    void * user_data);

extern WF_API void
wf_client_dispose(
    struct wf_client * client);

extern WF_API void *
wf_client_get_userdata(
    struct wf_client * client);

extern WF_API void
wf_client_service(
    struct wf_client * client);

extern WF_API void
wf_client_interrupt(
    struct wf_client * client);

extern WF_API void
wf_client_connect(
    struct wf_client * client,
    char const * url);

extern WF_API void
wf_client_disconnect(
    struct wf_client * client);

extern WF_API void
wf_client_authenticate(
    struct wf_client * client);

extern WF_API void
wf_client_add_filesystem(
    struct wf_client * client,
    char const * local_path,
    char const * name);

#ifdef __cplusplus
}
#endif

#endif