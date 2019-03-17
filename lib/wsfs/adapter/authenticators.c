#include "wsfs/adapter/authenticators.h"
#include <stddef.h>
#include <string.h>

#include "wsfs/adapter/authenticator.h"
#include "wsfs/adapter/credentials_intern.h"

static struct wsfs_authenticator * wsfs_authenticators_find(
    struct wsfs_authenticators * authenticators,
    char const * type)
{
    struct wsfs_authenticator * result = NULL;

    struct wsfs_authenticator * actual = authenticators->first;
    while ((NULL == result) && (NULL != actual))
    {
        struct wsfs_authenticator * next = actual->next;
        if (0 == strcmp(type, actual->type))
        {
            result = actual;
        }

        actual = next;
    }

    return result;
}

void wsfs_authenticators_init(
    struct wsfs_authenticators * authenticators)
{
    authenticators->first = NULL;
}

void wsfs_authenticators_cleanup(
    struct wsfs_authenticators * authenticators)
{
    struct wsfs_authenticator * actual = authenticators->first;
    while (NULL != actual)
    {
        struct wsfs_authenticator * next = actual->next;
        wsfs_authenticator_dispose(actual);
        actual = next;
    }

    authenticators->first = NULL;
}

void wsfs_authenticators_clone(
    struct wsfs_authenticators * authenticators,
    struct wsfs_authenticators * other)
{
    wsfs_authenticators_init(other);

    struct wsfs_authenticator * actual = authenticators->first;
    while (NULL != actual)
    {
        struct wsfs_authenticator * next = actual->next;
        wsfs_authenticators_add(other, 
                actual->type, actual->authenticate, actual->user_data);
        actual = next;
    }

}

void wsfs_authenticators_add(
    struct wsfs_authenticators * authenticators,
    char const * type,
    wsfs_authenticate_fn * authenticate,
    void * user_data)
{
    struct wsfs_authenticator * authenticator = wsfs_authenticator_create(type, authenticate, user_data);
    authenticator->next = authenticators->first;
    authenticators->first = authenticator;
}

bool wsfs_authenticators_authenticate(
    struct wsfs_authenticators * authenticators,
    struct wsfs_credentials * credentials)
{
    bool result;

    if (NULL != credentials)
    {
        struct wsfs_authenticator * authenticator = wsfs_authenticators_find(authenticators, credentials->type);
        if (NULL != authenticator)
        {
            result = wsfs_authenticator_autenticate(authenticator, credentials);
        }
        else 
        {
            result = false;
        }
    }
    else
    {
        result = (NULL == authenticators->first);
    }

    return result;
}