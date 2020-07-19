#ifndef WF_IMPL_JSON_NODE_INTERN_H
#define WF_IMPL_JSON_NODE_INTERN_H

#include "webfuse/impl/json/node.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_json_string
{
    char * data;
    size_t size;
};

struct wf_json_array
{
    struct wf_json * items;
    size_t size;
};

struct wf_json_object_item;

struct wf_json_object
{
    struct wf_json_object_item * items;
    size_t size;
};

union wf_json_value
{
    bool b;
    int i;
    struct wf_json_string s;
    struct wf_json_array a;
    struct wf_json_object o;
};

struct wf_json
{
    enum wf_json_type type;
    union wf_json_value value;
};

struct wf_json_object_item
{
    struct wf_json json;
    char * key;
};

extern void
wf_impl_json_cleanup(
    struct wf_json * json);

#ifdef __cplusplus
}
#endif

#endif
