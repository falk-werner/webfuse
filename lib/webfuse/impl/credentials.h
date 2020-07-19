#ifndef WF_ADAPTER_IMPL_CREDENTIALS_H
#define WF_ADAPTER_IMPL_CREDENTIALS_H

#ifndef __cplusplus
#include <stddef.h>
#else
#include <cstddef>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_json_writer;
struct wf_json;

struct wf_credentials_entry
{
    char * key;
    char * value;
};

struct wf_credentials
{
    char * type;
    struct wf_credentials_entry * entries;
    size_t capacity;
    size_t size;
};

extern void wf_impl_credentials_init(
    struct wf_credentials * credentials,
    char const * type,
    struct wf_json const * data);

extern void wf_impl_credentials_init_default(
    struct wf_credentials * credentials);

extern void wf_impl_credentials_cleanup(
    struct wf_credentials * credentials);

extern char const * wf_impl_credentials_type(
    struct wf_credentials const * credentials);

extern char const * wf_impl_credentials_get(
    struct wf_credentials const * credentials,
    char const * key);

extern void wf_impl_credentials_set_type(
    struct wf_credentials * credentials,
    char const * type);

extern void wf_impl_credentials_add(
    struct wf_credentials * credentials,
    char const * key,
    char const * value);

extern void
wf_impl_credentials_write(
    struct wf_json_writer * writer,
    void * data);

#ifdef __cplusplus
}
#endif

#endif