#include "wsfs/adapter/authenticator.h"

#include <stdlib.h>
#include <string.h>

#include "wsfs/adapter/credentials_intern.h"

struct wsfs_authenticator * wsfs_authenticator_create(
    char const * type,
    wsfs_authenticate_fn * authenticate,
    void * user_data)
{
    struct wsfs_authenticator * authenticator = malloc(sizeof(struct wsfs_authenticator));
    if (NULL != authenticator)
    {
        authenticator->type = strdup(type);
        authenticator->authenticate = authenticate;
        authenticator->user_data = user_data;
        authenticator->next = NULL;
    }

    return authenticator;
}

void wsfs_authenticator_dispose(
    struct wsfs_authenticator * authenticator)
{
    free(authenticator->type);
    free(authenticator);
}

bool wsfs_authenticator_autenticate(
    struct wsfs_authenticator * authenticator,
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