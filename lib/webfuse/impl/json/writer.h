#ifndef WF_IMPL_JSON_WRITER_H
#define WF_IMPL_JSON_WRITER_H

#ifndef __cplusplus
#include <stddef.h>
#include <stdbool.h>
#else
#include <cstddef>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_json_writer;

extern struct wf_json_writer *
wf_impl_json_writer_create(
    size_t initial_capacity,
    size_t pre);

extern void
wf_impl_json_writer_dispose(
    struct wf_json_writer * writer);

extern void
wf_impl_json_writer_reset(
    struct wf_json_writer * writer);

extern char *
wf_impl_json_writer_take(
    struct wf_json_writer * writer,
    size_t * size);

extern void
wf_impl_json_write_null(
    struct wf_json_writer * writer);

extern void
wf_impl_json_write_bool(
    struct wf_json_writer * writer,
    bool value);

extern void
wf_impl_json_write_int(
    struct wf_json_writer * writer,
    int value);

extern void
wf_impl_json_write_string(
    struct wf_json_writer * writer,
    char const * value);

extern void
wf_impl_json_write_string_nocheck(
    struct wf_json_writer * writer,
    char const * value);

extern void
wf_impl_json_write_bytes(
    struct wf_json_writer * writer,
    char const * data,
    size_t length);

extern void
wf_impl_json_write_array_begin(
    struct wf_json_writer * writer);

extern void
wf_impl_json_write_array_end(
    struct wf_json_writer * writer);

extern void
wf_impl_json_write_object_begin(
    struct wf_json_writer * writer);

extern void
wf_impl_json_write_object_end(
    struct wf_json_writer * writer);

extern void
wf_impl_json_write_object_key(
    struct wf_json_writer * writer,
    char const * key);

extern void
wf_impl_json_write_object_null(
    struct wf_json_writer * writer,
    char const * key);

extern void
wf_impl_json_write_object_bool(
    struct wf_json_writer * writer,
    char const * key,
    bool value);

extern void
wf_impl_json_write_object_int(
    struct wf_json_writer * writer,
    char const * key,
    int value);

extern void
wf_impl_json_write_object_string(
    struct wf_json_writer * writer,
    char const * key,
    char const * value);

extern void
wf_impl_json_write_object_string_nocheck(
    struct wf_json_writer * writer,
    char const * key,
    char const * value);

extern void
wf_impl_json_write_object_bytes(
    struct wf_json_writer * writer,
    char const * key,
    char const * data,
    size_t length);

extern void
wf_impl_json_write_object_begin_array(
    struct wf_json_writer * writer,
    char const * key);

extern void
wf_impl_json_write_object_begin_object(
    struct wf_json_writer * writer,
    char const * key);

#ifdef __cplusplus
}
#endif

#endif
