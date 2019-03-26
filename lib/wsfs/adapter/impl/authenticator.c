#include "wsfs/adapter/impl/authenticator.h"

#include <stdlib.h>
#include <string.h>

#include "wsfs/adapter/impl/credentials.h"

struct wsfs_impl_authenticator * wsfs_impl_authenticator_create(
    char const * type,
    wsfs_authenticate_fn * authenticate,
    void * user_data)
{
    struct wsfs_impl_authenticator * authenticator = malloc(sizeof(struct wsfs_impl_authenticator));
    if (NULL != authenticator)
    {
        authenticator->type = strdup(type);
        authenticator->authenticate = authenticate;
        authenticator->user_data = user_data;
        authenticator->next = NULL;
    }

    return authenticator;
}

void wsfs_impl_authenticator_dispose(
    struct wsfs_impl_authenticator * authenticator)
{
    free(authenticator->type);
    free(authenticator);
}

bool wsfs_impl_authenticator_autenticate(
    struct wsfs_impl_authenticator * authenticator,
    struct wsfs_credentials * credentials)
{
    bool result;

    if (0 == strcmp(authenticator->type, credentials->type))
    {
        result = authenticator->authenticate(credentials, authenticator->user_data);
    }
    else
    {
        result = false;
    }
    

    return result;
}