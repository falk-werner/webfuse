#include "wsfs/adapter/impl/authenticator.h"

#include <stdlib.h>
#include <string.h>

#include "wsfs/adapter/impl/credentials.h"

struct authenticator * authenticator_create(
    char const * type,
    wsfs_authenticate_fn * authenticate,
    void * user_data)
{
    struct authenticator * authenticator = malloc(sizeof(struct authenticator));
    if (NULL != authenticator)
    {
        authenticator->type = strdup(type);
        authenticator->authenticate = authenticate;
        authenticator->user_data = user_data;
        authenticator->next = NULL;
    }

    return authenticator;
}

void authenticator_dispose(
    struct authenticator * authenticator)
{
    free(authenticator->type);
    free(authenticator);
}

bool authenticator_autenticate(
    struct authenticator * authenticator,
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