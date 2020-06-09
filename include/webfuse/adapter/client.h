////////////////////////////////////////////////////////////////////////////////
/// \file adapter/client.h
/// \brief Adapter client.
////////////////////////////////////////////////////////////////////////////////

#ifndef WF_ADAPTER_CLIENT_H
#define WF_ADAPTER_CLIENT_H

#include "webfuse/adapter/api.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define WF_CLIENT_CREATED                      0x0001
#define WF_CLIENT_DISPOSING                    0x0002

#define WF_CLIENT_CONNECTED                    0x0011
#define WF_CLIENT_DISCONNECTED                 0x0012

#define WF_CLIENT_AUTHENTICATED                0x0021
#define WF_CLIENT_AUTHENTICATION_FAILED        0x0022
#define WF_CLIENT_AUTHENTICATE_GET_CREDENTIALS 0x0023

#define WF_CLIENT_FILESYSTEM_ADDED             0x0031
#define WF_CLIENT_FILESYSTEM_ADD_FAILED        0x0032

struct wf_client;

typedef void wf_client_callback_fn(
    struct wf_client * client,
    int reason,
    void * args);

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