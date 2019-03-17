#include "wsfs/adapter/credentials_intern.h"
#include <string.h>

void wsfs_credentials_init(
    struct wsfs_credentials * credentials,
    char const * type,
    json_t * data)
{
    credentials->type = strdup(type);
    credentials->data = data;
    json_incref(credentials->data);
}

void wsfs_credentials_cleanup(
    struct wsfs_credentials * credentials)
{
    free(credentials->type);
    json_decref(credentials->data);
}

char const * wsfs_credentials_type(
    struct wsfs_credentials const * credentials)
{
    return credentials->type;
}

char const * wsfs_credentials_get(
    struct wsfs_credentials const * credentials,
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
