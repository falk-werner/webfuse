#ifndef WSFS_ADAPTER_IMPL_AUTHENTICATORS_H
#define WSFS_ADAPTER_IMPL_AUTHENTICATORS_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include "wsfs/adapter/authenticate.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wsfs_impl_authenticator;
struct wsfs_credentials;

struct wsfs_impl_authenticators
{
    struct wsfs_impl_authenticator * first;
};

extern void wsfs_impl_authenticators_init(
    struct wsfs_impl_authenticators * authenticators);

extern void wsfs_impl_authenticators_cleanup(
    struct wsfs_impl_authenticators * authenticators);

extern void wsfs_impl_authenticators_clone(
    struct wsfs_impl_authenticators * authenticators,
    struct wsfs_impl_authenticators * other);

extern void wsfs_impl_authenticators_move(
    struct wsfs_impl_authenticators * authenticators,
    struct wsfs_impl_authenticators * other);

extern void wsfs_impl_authenticators_add(
    struct wsfs_impl_authenticators * authenticators,
    char const * type,
    wsfs_authenticate_fn * authenticate,
    void * user_data);

extern bool wsfs_impl_authenticators_authenticate(
    struct wsfs_impl_authenticators * authenticators,
    struct wsfs_credentials * credentials);

#ifdef __cplusplus
}
#endif

#endif
