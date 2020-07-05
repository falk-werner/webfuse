#include "webfuse/impl/util/buffer.h"
#include <stdlib.h>
#include <string.h>

void
wf_impl_buffer_init(
    struct wf_buffer * buffer,
    size_t initial_capacity)
{
    buffer->data = malloc(initial_capacity);
    buffer->size = 0;
    buffer->capacity = initial_capacity;
}

void
wf_impl_buffer_cleanup(
    struct wf_buffer * buffer)
{
    free(buffer->data);
}

bool
wf_impl_buffer_is_empty(
    struct wf_buffer * buffer)
{
    return (0 == buffer->size);
}

void
wf_impl_buffer_clear(
    struct wf_buffer * buffer)
{
    buffer->size = 0;
}

void
wf_impl_buffer_append(
    struct wf_buffer * buffer,
    char const * data,
    size_t length)
{
    while (length > (buffer->capacity - buffer->size))
    {
        buffer->capacity *= 2;
        buffer->data = realloc(buffer->data, buffer->capacity);
    }

    memcpy(&(buffer->data[buffer->size]), data, length);
    buffer->size += length;
}

char *
wf_impl_buffer_data(
    struct wf_buffer * buffer)
{
    return buffer->data;
}

size_t
wf_impl_buffer_size(
    struct wf_buffer * buffer)
{
    return buffer->size;
}
