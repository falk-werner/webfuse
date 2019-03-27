#ifndef WF_ADAPTER_IMPL_AUTHENTICATOR_H
#define WF_ADAPTER_IMPL_AUTHENTICATOR_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include "webfuse/adapter/authenticate.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_credentials;

struct wf_impl_authenticator
{
    char * type;
    wf_authenticate_fn * authenticate;
    void * user_data;
    struct wf_impl_authenticator * next;
};

extern struct wf_impl_authenticator * wf_impl_authenticator_create(
    char const * type,
    wf_authenticate_fn * authenticate,
    void * user_data);

extern void wf_impl_authenticator_dispose(
    struct wf_impl_authenticator * authenticator);

extern bool wf_impl_authenticator_autenticate(
    struct wf_impl_authenticator * authenticator,
    struct wf_credentials * credentials);



#ifdef __cplusplus
}
#endif
#endif
