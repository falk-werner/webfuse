#include "wsfs/adapter/impl/credentials.h"
#include <string.h>

void credentials_init(
    struct credentials * credentials,
    char const * type,
    json_t * data)
{
    credentials->type = strdup(type);
    credentials->data = data;
    json_incref(credentials->data);
}

void credentials_cleanup(
    struct credentials * credentials)
{
    free(credentials->type);
    json_decref(credentials->data);
}

char const * credentials_type(
    struct credentials const * credentials)
{
    return credentials->type;
}

char const * credentials_get(
    struct credentials const * credentials,
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
