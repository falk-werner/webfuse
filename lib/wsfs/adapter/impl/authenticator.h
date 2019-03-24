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

struct authenticator
{
    char * type;
    wsfs_authenticate_fn * authenticate;
    void * user_data;
    struct authenticator * next;
};

extern struct authenticator * authenticator_create(
    char const * type,
    wsfs_authenticate_fn * authenticate,
    void * user_data);

extern void authenticator_dispose(
    struct authenticator * authenticator);

extern bool authenticator_autenticate(
    struct authenticator * authenticator,
    struct wsfs_credentials * credentials);



#ifdef __cplusplus
}
#endif
#endif
