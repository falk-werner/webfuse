#ifndef WF_IMPL_UTIL_BUFFER_H
#define WF_IMPL_UTIL_BUFFER_H

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

struct wf_buffer
{
    char * data;
    size_t size;
    size_t capacity;
};

extern void
wf_impl_buffer_init(
    struct wf_buffer * buffer,
    size_t initial_capacity);

extern void
wf_impl_buffer_cleanup(
    struct wf_buffer * buffer);

extern bool
wf_impl_buffer_is_empty(
    struct wf_buffer * buffer);

extern void
wf_impl_buffer_clear(
    struct wf_buffer * buffer);

extern void
wf_impl_buffer_append(
    struct wf_buffer * buffer,
    char const * data,
    size_t lenght);

extern char *
wf_impl_buffer_data(
    struct wf_buffer * buffer);

extern size_t
wf_impl_buffer_size(
    struct wf_buffer * buffer);


#ifdef __cplusplus
}
#endif

#endif
