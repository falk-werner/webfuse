#ifndef WF_IMPL_JSON_READER_H
#define WF_IMPL_JSON_READER_H

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

struct wf_json_reader
{
    char * contents;
    size_t length;
    size_t pos;
};

extern void
wf_impl_json_reader_init(
    struct wf_json_reader * reader,
    char * contents,
    size_t length);

extern char
wf_impl_json_reader_skip_whitespace(
    struct wf_json_reader * reader);

extern char
wf_impl_json_reader_peek(
    struct wf_json_reader * reader);

extern char
wf_impl_json_reader_get_char(
    struct wf_json_reader * reader);

extern void
wf_impl_json_reader_unget_char(
    struct wf_json_reader * reader);

extern bool
wf_impl_json_reader_read_const(
    struct wf_json_reader * reader,
    char const * value,
    size_t length);

extern bool
wf_impl_json_reader_read_int(
    struct wf_json_reader * reader,
    int * value);

extern bool
wf_impl_json_reader_read_string(
    struct wf_json_reader * reader,
    char * * value);

#ifdef __cplusplus
}
#endif

#endif
