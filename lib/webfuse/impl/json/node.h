#ifndef WF_IMPL_JSON_NODE_H
#define WF_IMPL_JSON_NODE_H

#ifndef __cplusplus
#include <stdbool.h>
#include <stddef.h>
#else
#include <cstddef>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

enum wf_json_type
{
    WF_JSON_TYPE_UNDEFINED,
    WF_JSON_TYPE_NULL,
    WF_JSON_TYPE_BOOL,
    WF_JSON_TYPE_INT,
    WF_JSON_TYPE_STRING,
    WF_JSON_TYPE_ARRAY,
    WF_JSON_TYPE_OBJECT
};

struct wf_json;

extern enum wf_json_type
wf_impl_json_type(
    struct wf_json const * json);

extern bool
wf_impl_json_bool_get(
    struct wf_json const * json);

extern int
wf_impl_json_int_get(
    struct wf_json const * json);

extern char const *
wf_impl_json_string_get(
    struct wf_json const * json);

extern size_t
wf_impl_json_string_size(
    struct wf_json const * json);

extern struct wf_json const *
wf_impl_json_array_get(
    struct wf_json const * json,
    size_t pos);

extern size_t
wf_impl_json_array_size(
    struct wf_json const * json);

extern struct wf_json const *
wf_impl_json_object_get(
    struct wf_json const * json,
    char const * key);

extern size_t
wf_impl_json_object_size(
    struct wf_json const * json);

extern char const *
wf_impl_json_object_key(
    struct wf_json const * json,
    size_t pos);

extern struct wf_json const *
wf_impl_json_object_value(
    struct wf_json const * json,
    size_t pos);

#ifdef __cplusplus
}
#endif

#endif
