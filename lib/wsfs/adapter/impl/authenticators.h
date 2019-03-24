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

struct authenticator;
struct wsfs_credentials;

struct authenticators
{
    struct authenticator * first;
};

extern void authenticators_init(
    struct authenticators * authenticators);

extern void authenticators_cleanup(
    struct authenticators * authenticators);

extern void authenticators_clone(
    struct authenticators * authenticators,
    struct authenticators * other);

extern void authenticators_move(
    struct authenticators * authenticators,
    struct authenticators * other);

extern void authenticators_add(
    struct authenticators * authenticators,
    char const * type,
    wsfs_authenticate_fn * authenticate,
    void * user_data);

extern bool authenticators_authenticate(
    struct authenticators * authenticators,
    struct wsfs_credentials * credentials);

#ifdef __cplusplus
}
#endif

#endif
