#include "webfuse/adapter/impl/authenticators.h"
#include <stddef.h>
#include <string.h>

#include "webfuse/adapter/impl/authenticator.h"
#include "webfuse/adapter/impl/credentials.h"

static struct wf_impl_authenticator * wf_impl_authenticators_find(
    struct wf_impl_authenticators * authenticators,
    char const * type)
{
    struct wf_impl_authenticator * result = NULL;

    struct wf_impl_authenticator * actual = authenticators->first;
    while ((NULL == result) && (NULL != actual))
    {
        struct wf_impl_authenticator * next = actual->next;
        if (0 == strcmp(type, actual->type))
        {
            result = actual;
        }

        actual = next;
    }

    return result;
}

void wf_impl_authenticators_init(
    struct wf_impl_authenticators * authenticators)
{
    authenticators->first = NULL;
}

void wf_impl_authenticators_cleanup(
    struct wf_impl_authenticators * authenticators)
{
    struct wf_impl_authenticator * actual = authenticators->first;
    while (NULL != actual)
    {
        struct wf_impl_authenticator * next = actual->next;
        wf_impl_authenticator_dispose(actual);
        actual = next;
    }

    authenticators->first = NULL;
}

void wf_impl_authenticators_clone(
    struct wf_impl_authenticators * authenticators,
    struct wf_impl_authenticators * other)
{
    wf_impl_authenticators_init(other);

    struct wf_impl_authenticator * actual = authenticators->first;
    while (NULL != actual)
    {
        struct wf_impl_authenticator * next = actual->next;
        wf_impl_authenticators_add(other, 
                actual->type, actual->authenticate, actual->user_data);
        actual = next;
    }

}

extern void wf_impl_authenticators_move(
    struct wf_impl_authenticators * authenticators,
    struct wf_impl_authenticators * other)
{
    other->first = authenticators->first;
    authenticators->first = NULL;
}

void wf_impl_authenticators_add(
    struct wf_impl_authenticators * authenticators,
    char const * type,
    wf_authenticate_fn * authenticate,
    void * user_data)
{
    struct wf_impl_authenticator * authenticator = wf_impl_authenticator_create(type, authenticate, user_data);
    authenticator->next = authenticators->first;
    authenticators->first = authenticator;
}

bool wf_impl_authenticators_authenticate(
    struct wf_impl_authenticators * authenticators,
    struct wf_credentials * credentials)
{
    bool result =  (NULL == authenticators->first);

    if (NULL != credentials)
    {
        struct wf_impl_authenticator * authenticator = wf_impl_authenticators_find(authenticators, credentials->type);
        if (NULL != authenticator)
        {
            result = wf_impl_authenticator_autenticate(authenticator, credentials);
        }
    }

    return result;
}