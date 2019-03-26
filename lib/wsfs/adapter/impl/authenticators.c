#include "wsfs/adapter/impl/authenticators.h"
#include <stddef.h>
#include <string.h>

#include "wsfs/adapter/impl/authenticator.h"
#include "wsfs/adapter/impl/credentials.h"

static struct wsfs_impl_authenticator * wsfs_impl_authenticators_find(
    struct wsfs_impl_authenticators * authenticators,
    char const * type)
{
    struct wsfs_impl_authenticator * result = NULL;

    struct wsfs_impl_authenticator * actual = authenticators->first;
    while ((NULL == result) && (NULL != actual))
    {
        struct wsfs_impl_authenticator * next = actual->next;
        if (0 == strcmp(type, actual->type))
        {
            result = actual;
        }

        actual = next;
    }

    return result;
}

void wsfs_impl_authenticators_init(
    struct wsfs_impl_authenticators * authenticators)
{
    authenticators->first = NULL;
}

void wsfs_impl_authenticators_cleanup(
    struct wsfs_impl_authenticators * authenticators)
{
    struct wsfs_impl_authenticator * actual = authenticators->first;
    while (NULL != actual)
    {
        struct wsfs_impl_authenticator * next = actual->next;
        wsfs_impl_authenticator_dispose(actual);
        actual = next;
    }

    authenticators->first = NULL;
}

void wsfs_impl_authenticators_clone(
    struct wsfs_impl_authenticators * authenticators,
    struct wsfs_impl_authenticators * other)
{
    wsfs_impl_authenticators_init(other);

    struct wsfs_impl_authenticator * actual = authenticators->first;
    while (NULL != actual)
    {
        struct wsfs_impl_authenticator * next = actual->next;
        wsfs_impl_authenticators_add(other, 
                actual->type, actual->authenticate, actual->user_data);
        actual = next;
    }

}

extern void wsfs_impl_authenticators_move(
    struct wsfs_impl_authenticators * authenticators,
    struct wsfs_impl_authenticators * other)
{
    other->first = authenticators->first;
    authenticators->first = NULL;
}

void wsfs_impl_authenticators_add(
    struct wsfs_impl_authenticators * authenticators,
    char const * type,
    wsfs_authenticate_fn * authenticate,
    void * user_data)
{
    struct wsfs_impl_authenticator * authenticator = wsfs_impl_authenticator_create(type, authenticate, user_data);
    authenticator->next = authenticators->first;
    authenticators->first = authenticator;
}

bool wsfs_impl_authenticators_authenticate(
    struct wsfs_impl_authenticators * authenticators,
    struct wsfs_credentials * credentials)
{
    bool result =  (NULL == authenticators->first);

    if (NULL != credentials)
    {
        struct wsfs_impl_authenticator * authenticator = wsfs_impl_authenticators_find(authenticators, credentials->type);
        if (NULL != authenticator)
        {
            result = wsfs_impl_authenticator_autenticate(authenticator, credentials);
        }
    }

    return result;
}