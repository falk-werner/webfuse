#ifndef WF_ADAPTER_IMPL_CLIENT_H
#define WF_ADAPTER_IMPL_CLIENT_H


#include "webfuse/adapter/client_callback.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern struct wf_client *
wf_impl_client_create(
    wf_client_callback_fn * callback,
    void * user_data);

extern void
wf_impl_client_dispose(
    struct wf_client * client);

extern void *
wf_impl_client_get_userdata(
    struct wf_client * client);

extern void
wf_impl_client_service(
    struct wf_client * client);

extern void
wf_impl_client_interrupt(
    struct wf_client * client);

extern void
wf_impl_client_connect(
    struct wf_client * client,
    char const * url);

extern void
wf_impl_client_disconnect(
    struct wf_client * client);

extern void
wf_impl_client_authenticate(
    struct wf_client * client);

extern void
wf_impl_client_add_filesystem(
    struct wf_client * client,
    char const * local_path,
    char const * name);


#ifdef __cplusplus
}
#endif

#endif
