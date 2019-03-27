#include "webfuse/adapter/impl/credentials.h"
#include <string.h>

void wf_impl_credentials_init(
    struct wf_credentials * credentials,
    char const * type,
    json_t * data)
{
    credentials->type = strdup(type);
    credentials->data = data;
    json_incref(credentials->data);
}

void wf_impl_credentials_cleanup(
    struct wf_credentials * credentials)
{
    free(credentials->type);
    json_decref(credentials->data);
}

char const * wf_impl_credentials_type(
    struct wf_credentials const * credentials)
{
    return credentials->type;
}

char const * wf_impl_credentials_get(
    struct wf_credentials const * credentials,
    char const * key)
{
    char const * result = NULL;

    json_t * value_holder = json_object_get(credentials->data, key);
    if (json_is_string(value_holder))
    {
        result = json_string_value(value_holder);
    }

    return result;
}
