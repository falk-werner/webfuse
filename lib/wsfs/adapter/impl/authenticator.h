#ifndef WSFS_ADAPTER_IMPL_AUTHENTICATOR_H
#define WSFS_ADAPTER_IMPL_AUTHENTICATOR_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include "wsfs/adapter/authenticate.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wsfs_credentials;

struct wsfs_impl_authenticator
{
    char * type;
    wsfs_authenticate_fn * authenticate;
    void * user_data;
    struct wsfs_impl_authenticator * next;
};

extern struct wsfs_impl_authenticator * wsfs_impl_authenticator_create(
    char const * type,
    wsfs_authenticate_fn * authenticate,
    void * user_data);

extern void wsfs_impl_authenticator_dispose(
    struct wsfs_impl_authenticator * authenticator);

extern bool wsfs_impl_authenticator_autenticate(
    struct wsfs_impl_authenticator * authenticator,
    struct wsfs_credentials * credentials);



#ifdef __cplusplus
}
#endif
#endif
