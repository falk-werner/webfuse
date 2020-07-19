#include "webfuse/impl/json/node_intern.h"

#include <stdlib.h>
#include <string.h>

static struct wf_json const wf_json_undefined =
{
    .type = WF_JSON_TYPE_UNDEFINED,
    .value = { .b = false }
};

enum wf_json_type
wf_impl_json_type(
    struct wf_json const * json)
{
    return json->type;
}

bool
wf_impl_json_bool_get(
    struct wf_json const * json)
{
    return (WF_JSON_TYPE_BOOL == json->type) ? json->value.b : false;
}

bool
wf_impl_json_is_undefined(
    struct wf_json const * json)
{
    return (WF_JSON_TYPE_UNDEFINED == json->type);
}

bool
wf_impl_json_is_null(
    struct wf_json const * json)
{
    return (WF_JSON_TYPE_NULL == json->type);
}

bool
wf_impl_json_is_bool(
    struct wf_json const * json)
{
    return (WF_JSON_TYPE_BOOL == json->type);
}

bool
wf_impl_json_is_int(
    struct wf_json const * json)
{
    return (WF_JSON_TYPE_INT == json->type);
}

bool
wf_impl_json_is_string(
    struct wf_json const * json)
{
    return (WF_JSON_TYPE_STRING == json->type);
}

bool
wf_impl_json_is_array(
    struct wf_json const * json)
{
    return (WF_JSON_TYPE_ARRAY == json->type);
}

bool
wf_impl_json_is_object(
    struct wf_json const * json)
{
    return (WF_JSON_TYPE_OBJECT == json->type);
}

int
wf_impl_json_int_get(
    struct wf_json const * json)
{
    return (WF_JSON_TYPE_INT == json->type) ? json->value.i : 0;
}

char const *
wf_impl_json_string_get(
    struct wf_json const * json)
{
    return (WF_JSON_TYPE_STRING == json->type) ? json->value.s.data : "";
}

size_t
wf_impl_json_string_size(
    struct wf_json const * json)
{
    return (WF_JSON_TYPE_STRING == json->type) ? json->value.s.size : 0;
}


struct wf_json const *
wf_impl_json_array_get(
    struct wf_json const * json,
    size_t pos)
{
    struct wf_json const * result = &wf_json_undefined;
    if ((WF_JSON_TYPE_ARRAY == json->type) && (pos < json->value.a.size))
    {
        result = &(json->value.a.items[pos]);
    }

    return result;
}

size_t
wf_impl_json_array_size(
    struct wf_json const * json)
{
    return (WF_JSON_TYPE_ARRAY == json->type) ? json->value.a.size : 0;
}

struct wf_json const *
wf_impl_json_object_get(
    struct wf_json const * json,
    char const * key)
 {
    if (WF_JSON_TYPE_OBJECT == json->type)
    {
        size_t const count = json->value.o.size;
        for(size_t i = 0; i < count; i++)
        {
            struct wf_json_object_item * actual = &(json->value.o.items[i]);
            if (0 == strcmp(key, actual->key))
            {
                return &(actual->json);
            }
        }
    }

    return &wf_json_undefined;
 }

size_t
wf_impl_json_object_size(
    struct wf_json const * json)
{
    return (WF_JSON_TYPE_OBJECT == json->type) ? json->value.o.size : 0;
}

char const *
wf_impl_json_object_key(
    struct wf_json const * json,
    size_t pos)
{
    char const * result = "";
    if ((WF_JSON_TYPE_OBJECT == json->type) && (pos < json->value.o.size))
    {
        result = json->value.o.items[pos].key;
    }

    return result;
}

struct wf_json const *
wf_impl_json_object_value(
    struct wf_json const * json,
    size_t pos)
{
    struct wf_json const * result = &wf_json_undefined;
    if ((WF_JSON_TYPE_OBJECT == json->type) && (pos < json->value.o.size))
    {
        result = &(json->value.o.items[pos].json);
    }

    return result;

}

void
wf_impl_json_cleanup(
    struct wf_json * json)
{
    switch (json->type)
    {
        case WF_JSON_TYPE_ARRAY:
            {
                size_t const count = json->value.a.size;
                for(size_t i = 0; i < count; i++)
                {
                    struct wf_json * actual = &(json->value.a.items[i]);
                    wf_impl_json_cleanup(actual);
                }
                free(json->value.a.items);
            }
            break;
        case WF_JSON_TYPE_OBJECT:
            {
                size_t const count = json->value.o.size;
                for(size_t i = 0; i < count; i++)
                {
                    struct wf_json * actual = &(json->value.o.items[i].json);
                    wf_impl_json_cleanup(actual);
                }
                free(json->value.o.items);
            }
            break;
        default:
            break;
    }
}
