#include "webfuse/provider/impl/credentials.h"

#include <stdlib.h>
#include <string.h>

void  wfp_impl_credentials_init(
    struct wfp_credentials * credentials)
{
    credentials->type = NULL;
    credentials->contents = json_object();
}

void wfp_impl_credentials_cleanup(
    struct wfp_credentials * credentials)
{
    free(credentials->type);
    json_decref(credentials->contents);
}

void wfp_impl_credentials_set_type(
    struct wfp_credentials * credentials,
    char const * type)
{
    free(credentials->type);
    credentials->type = strdup(type);
}

void wfp_impl_credentials_add(
    struct wfp_credentials * credentials,
    char const * key,
    char const * value)
{
    json_object_set_new(credentials->contents, key, json_string(value));
}

char const * wfp_impl_credentials_get_type(
    struct wfp_credentials * credentials)
{
    return credentials->type;
}

json_t * wfp_impl_credentials_get(
    struct wfp_credentials * credentials)
{
    return credentials->contents;
}
