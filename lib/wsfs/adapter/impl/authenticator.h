#ifndef WSFS_ADAPTER_IMPL_AUTHENTICATOR_H
#define WSFS_ADAPTER_IMPL_AUTHENTICATOR_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include "wsfs/adapter/impl/authenticate.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct credentials;

struct authenticator
{
    char * type;
    authenticate_fn * authenticate;
    void * user_data;
    struct authenticator * next;
};

extern struct authenticator * authenticator_create(
    char const * type,
    authenticate_fn * authenticate,
    void * user_data);

extern void authenticator_dispose(
    struct authenticator * authenticator);

extern bool authenticator_autenticate(
    struct authenticator * authenticator,
    struct credentials * credentials);



#ifdef __cplusplus
}
#endif
#endif
