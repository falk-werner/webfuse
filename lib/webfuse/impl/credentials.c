#include "webfuse/impl/credentials.h"
#include "webfuse/impl/json/writer.h"
#include "webfuse/impl/json/node.h"

#include <stdlib.h>
#include <string.h>

#define WF_CREDENTIALS_INITIAL_CAPACITY 2

void wf_impl_credentials_init_default(
    struct wf_credentials * credentials)
{
    credentials->type = NULL;
    credentials->capacity = WF_CREDENTIALS_INITIAL_CAPACITY;
    credentials->entries = malloc(sizeof(struct wf_credentials_entry) * credentials->capacity);
    credentials->size = 0;
}

void wf_impl_credentials_init(
    struct wf_credentials * credentials,
    char const * type,
    struct wf_json const * data)
{
    size_t count = wf_impl_json_object_size(data);
    credentials->type = strdup(type);
    credentials->capacity = (count > 0) ? count : WF_CREDENTIALS_INITIAL_CAPACITY;
    credentials->entries = malloc(sizeof(struct wf_credentials_entry) * credentials->capacity);
    credentials->size = 0;

    for (size_t i; i < count; i++)
    {
        char const * key = wf_impl_json_object_key(data, i);
        struct wf_json const * value = wf_impl_json_object_value(data, i);

        wf_impl_credentials_add(credentials, key, wf_impl_json_string_get(value));
    }
}

void wf_impl_credentials_cleanup(
    struct wf_credentials * credentials)
{
    for (size_t i = 0; i < credentials->size; i++)
    {
        free(credentials->entries[i].key);
        free(credentials->entries[i].value);
    }
    free(credentials->entries);
    free(credentials->type);
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
    for (size_t i = 0; i < credentials->size; i++)
    {
        if (0 == strcmp(key, credentials->entries[i].key))
        {
            return credentials->entries[i].value;
        }
    }

    return NULL;
}

void wf_impl_credentials_set_type(
    struct wf_credentials * credentials,
    char const * type)
{
    free(credentials->type);
    credentials->type = strdup(type);
}

void wf_impl_credentials_add(
    struct wf_credentials * credentials,
    char const * key,
    char const * value)
{
    if (credentials->size >= credentials->capacity)
    {
        credentials->capacity *= 2;
        credentials->entries = realloc(credentials->entries, credentials->capacity);
    }

    credentials->entries[credentials->size].key = strdup(key);
    credentials->entries[credentials->size].value = strdup(value);
    credentials->size++;
}

void
wf_impl_credentials_write(
    struct wf_json_writer * writer,
    void * data)
{
    struct wf_credentials * credentials = data;

    wf_impl_json_write_object_begin(writer);
    for (size_t i = 0; i < credentials->size; i++)
    {
        char const * key = credentials->entries[i].key;
        char const * value = credentials->entries[i].value;

        wf_impl_json_write_object_string(writer, key, value);
    }
    wf_impl_json_write_object_end(writer);
}
