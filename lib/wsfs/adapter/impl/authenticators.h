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

struct wsfs_authenticator;
struct wsfs_credentials;

struct wsfs_authenticators
{
    struct wsfs_authenticator * first;
};

extern void wsfs_authenticators_init(
    struct wsfs_authenticators * authenticators);

extern void wsfs_authenticators_cleanup(
    struct wsfs_authenticators * authenticators);

extern void wsfs_authenticators_clone(
    struct wsfs_authenticators * authenticators,
    struct wsfs_authenticators * other);

extern void wsfs_authenticators_move(
    struct wsfs_authenticators * authenticators,
    struct wsfs_authenticators * other);

extern void wsfs_authenticators_add(
    struct wsfs_authenticators * authenticators,
    char const * type,
    wsfs_authenticate_fn * authenticate,
    void * user_data);

extern bool wsfs_authenticators_authenticate(
    struct wsfs_authenticators * authenticators,
    struct wsfs_credentials * credentials);

#ifdef __cplusplus
}
#endif

#endif
