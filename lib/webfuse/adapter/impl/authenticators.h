#ifndef WF_ADAPTER_IMPL_AUTHENTICATORS_H
#define WF_ADAPTER_IMPL_AUTHENTICATORS_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include "webfuse/adapter/authenticate.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_impl_authenticator;
struct wf_credentials;

struct wf_impl_authenticators
{
    struct wf_impl_authenticator * first;
};

extern void wf_impl_authenticators_init(
    struct wf_impl_authenticators * authenticators);

extern void wf_impl_authenticators_cleanup(
    struct wf_impl_authenticators * authenticators);

extern void wf_impl_authenticators_clone(
    struct wf_impl_authenticators * authenticators,
    struct wf_impl_authenticators * other);

extern void wf_impl_authenticators_move(
    struct wf_impl_authenticators * authenticators,
    struct wf_impl_authenticators * other);

extern void wf_impl_authenticators_add(
    struct wf_impl_authenticators * authenticators,
    char const * type,
    wf_authenticate_fn * authenticate,
    void * user_data);

extern bool wf_impl_authenticators_authenticate(
    struct wf_impl_authenticators * authenticators,
    struct wf_credentials * credentials);

#ifdef __cplusplus
}
#endif

#endif
