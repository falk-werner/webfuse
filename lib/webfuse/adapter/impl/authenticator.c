#include "webfuse/adapter/impl/authenticator.h"

#include <stdlib.h>
#include <string.h>

#include "webfuse/adapter/impl/credentials.h"

struct wf_impl_authenticator * wf_impl_authenticator_create(
    char const * type,
    wf_authenticate_fn * authenticate,
    void * user_data)
{
    struct wf_impl_authenticator * authenticator = malloc(sizeof(struct wf_impl_authenticator));
    authenticator->type = strdup(type);
    authenticator->authenticate = authenticate;
    authenticator->user_data = user_data;
    authenticator->next = NULL;

    return authenticator;
}

void wf_impl_authenticator_dispose(
    struct wf_impl_authenticator * authenticator)
{
    free(authenticator->type);
    free(authenticator);
}

bool wf_impl_authenticator_autenticate(
    struct wf_impl_authenticator * authenticator,
    struct wf_credentials * credentials)
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