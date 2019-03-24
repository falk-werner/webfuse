#include "wsfs/adapter/impl/authenticators.h"
#include <stddef.h>
#include <string.h>

#include "wsfs/adapter/impl/authenticator.h"
#include "wsfs/adapter/impl/credentials.h"

static struct authenticator * authenticators_find(
    struct authenticators * authenticators,
    char const * type)
{
    struct authenticator * result = NULL;

    struct authenticator * actual = authenticators->first;
    while ((NULL == result) && (NULL != actual))
    {
        struct authenticator * next = actual->next;
        if (0 == strcmp(type, actual->type))
        {
            result = actual;
        }

        actual = next;
    }

    return result;
}

void authenticators_init(
    struct authenticators * authenticators)
{
    authenticators->first = NULL;
}

void authenticators_cleanup(
    struct authenticators * authenticators)
{
    struct authenticator * actual = authenticators->first;
    while (NULL != actual)
    {
        struct authenticator * next = actual->next;
        authenticator_dispose(actual);
        actual = next;
    }

    authenticators->first = NULL;
}

void authenticators_clone(
    struct authenticators * authenticators,
    struct authenticators * other)
{
    authenticators_init(other);

    struct authenticator * actual = authenticators->first;
    while (NULL != actual)
    {
        struct authenticator * next = actual->next;
        authenticators_add(other, 
                actual->type, actual->authenticate, actual->user_data);
        actual = next;
    }

}

extern void authenticators_move(
    struct authenticators * authenticators,
    struct authenticators * other)
{
    other->first = authenticators->first;
    authenticators->first = NULL;
}

void authenticators_add(
    struct authenticators * authenticators,
    char const * type,
    wsfs_authenticate_fn * authenticate,
    void * user_data)
{
    struct authenticator * authenticator = authenticator_create(type, authenticate, user_data);
    authenticator->next = authenticators->first;
    authenticators->first = authenticator;
}

bool authenticators_authenticate(
    struct authenticators * authenticators,
    struct wsfs_credentials * credentials)
{
    bool result =  (NULL == authenticators->first);

    if (NULL != credentials)
    {
        struct authenticator * authenticator = authenticators_find(authenticators, credentials->type);
        if (NULL != authenticator)
        {
            result = authenticator_autenticate(authenticator, credentials);
        }
    }

    return result;
}