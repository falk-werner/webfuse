#include "webfuse/impl/json/writer.h"
#include "webfuse/impl/util/base64.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define WF_JSON_WRITER_INITIAL_MAX_LEVEL 7

#define WF_JSON_WRITER_SEPARATOR_SIZE            ( 1)
#define WF_JSON_WRITER_INT_SIZE                   (31 + WF_JSON_WRITER_SEPARATOR_SIZE)
#define WF_JSON_WRITER_NULL_SIZE                  ( 4 + WF_JSON_WRITER_SEPARATOR_SIZE)
#define WF_JSON_WRITER_BOOL_SIZE                  ( 5 + WF_JSON_WRITER_SEPARATOR_SIZE)
#define WF_JSON_WRITER_ADDITIONAL_STRING_SIZE     ( 2 + WF_JSON_WRITER_SEPARATOR_SIZE)
#define WF_JSON_WRITER_BEGIN_ARRAY_SIZE           ( 1 + WF_JSON_WRITER_SEPARATOR_SIZE)
#define WF_JSON_WRITER_END_ARRAY_SIZE             ( 1)
#define WF_JSON_WRITER_BEGIN_OBJECT_SIZE          ( 1 + WF_JSON_WRITER_SEPARATOR_SIZE)
#define WF_JSON_WRITER_END_OBJECT_SIZE            ( 1)
#define WF_JSON_WRITER_ADDITIONAL_OBJECT_KEY_SIZE ( 4)

enum wf_json_writer_state
{
    WF_JSON_WRITER_STATE_INIT,
    WF_JSON_WRITER_STATE_ARRAY_FIRST,
    WF_JSON_WRITER_STATE_ARRAY_NEXT,
    WF_JSON_WRITER_STATE_OBJECT_FIRST,
    WF_JSON_WRITER_STATE_OBJECT_NEXT
};

struct wf_json_writer
{
    enum wf_json_writer_state * state;
    size_t max_level;
    size_t level;
    size_t capacity;
    size_t pre;
    size_t offset;
    char * data;
    char * raw_data;
};

// --

static char
wf_impl_json_writer_get_esc(
    char c);

static void
wf_impl_json_write_raw_char(
    struct wf_json_writer * writer,
    char value);

static void
wf_impl_json_write_raw(
    struct wf_json_writer * writer,
    char const * value,
    size_t length);

static void
wf_impl_json_reserve(
    struct wf_json_writer * writer,
    size_t needed);

static void
wf_impl_json_begin_value(
    struct wf_json_writer * writer);

static void
wf_impl_json_end_value(
    struct wf_json_writer * writer);

static void
wf_impl_json_push_state(
    struct wf_json_writer * writer,
    enum wf_json_writer_state state);

static void
wf_impl_json_pop_state(
    struct wf_json_writer * writer);

// --

struct wf_json_writer *
wf_impl_json_writer_create(
    size_t initial_capacity,
    size_t pre)
{
    struct wf_json_writer * writer = malloc(sizeof(struct wf_json_writer));
    writer->level = 0;
    writer->max_level = WF_JSON_WRITER_INITIAL_MAX_LEVEL;
    writer->state = malloc((1 + writer->max_level) * sizeof(enum wf_json_writer_state));
    writer->state[writer->level] = WF_JSON_WRITER_STATE_INIT;
    writer->pre = pre;
    writer->offset = 0;
    writer->capacity = initial_capacity;
    writer->raw_data = malloc(writer->pre + writer->capacity);
    writer->data = &(writer->raw_data[pre]);

    return writer;
}

void
wf_impl_json_writer_dispose(
    struct wf_json_writer * writer)
{
    free(writer->raw_data);
    free(writer->state);
    free(writer);
}

void
wf_impl_json_writer_reset(
    struct wf_json_writer * writer)
{
    writer->level = 0;
    writer->state[writer->level] = WF_JSON_WRITER_STATE_INIT;
    writer->offset = 0;
}

char *
wf_impl_json_writer_take(
    struct wf_json_writer * writer,
    size_t * size)
{
    wf_impl_json_reserve(writer, 1);
    writer->data[writer->offset] = '\0';

    writer->raw_data = NULL;

    if (NULL != size)
    {
        *size = writer->offset;
    }

    return writer->data;
}

void
wf_impl_json_write_null(
    struct wf_json_writer * writer)
{
    wf_impl_json_reserve(writer, WF_JSON_WRITER_NULL_SIZE);
    wf_impl_json_begin_value(writer);
    wf_impl_json_write_raw(writer, "null", 4);
    wf_impl_json_end_value(writer);
}

void
wf_impl_json_write_bool(
    struct wf_json_writer * writer,
    bool value)
{
    wf_impl_json_reserve(writer, WF_JSON_WRITER_BOOL_SIZE);
    wf_impl_json_begin_value(writer);

    if (value)
    {
        wf_impl_json_write_raw(writer, "true", 4);
    }
    else
    {
        wf_impl_json_write_raw(writer, "false", 5);
    }

    wf_impl_json_end_value(writer);
}

void
wf_impl_json_write_int(
    struct wf_json_writer * writer,
    int value)
{
    wf_impl_json_reserve(writer, WF_JSON_WRITER_INT_SIZE);
    wf_impl_json_begin_value(writer);

    bool const is_signed = (0 > value);
    char buffer[WF_JSON_WRITER_INT_SIZE];
    size_t offset = WF_JSON_WRITER_INT_SIZE;
    buffer[--offset] = '\0';
    if (is_signed)
    {
        if (INT_MIN == value)
        {
            char const actual = (char) abs(value % 10);
            buffer[--offset] = (char) ('0' + actual);
            value /= 10;
        }

        value =- value;
    }

    do 
    {
        char const actual = (char) (value % 10);
        buffer[--offset] = (char) ('0' + actual);
        value /= 10;
    } while (0 != value);

    if (is_signed)
    {
        buffer[--offset] = '-';
    }

    size_t const length = (WF_JSON_WRITER_INT_SIZE - offset - 1);
    wf_impl_json_write_raw(writer, &(buffer[offset]), length);

    wf_impl_json_end_value(writer);
}

void
wf_impl_json_write_string(
    struct wf_json_writer * writer,
    char const * value)
{
    size_t length = strlen(value);
    wf_impl_json_reserve(writer, length + WF_JSON_WRITER_ADDITIONAL_STRING_SIZE);
    wf_impl_json_begin_value(writer);

    wf_impl_json_write_raw_char(writer, '\"');
    for (size_t i = 0; i < length; i++)
    {
        char const c = value[i];
        if ((' ' <= c) && ('\\' != c) && ('\"' != c))
        {
            wf_impl_json_write_raw_char(writer, c);
        }
        else
        {
            char esc = wf_impl_json_writer_get_esc(c);

            wf_impl_json_reserve(writer, (length - i) + 2);
            wf_impl_json_write_raw_char(writer, '\\');
            wf_impl_json_write_raw_char(writer, esc);
        }
        
    }
    wf_impl_json_write_raw_char(writer, '\"');

    wf_impl_json_end_value(writer);
}

void
wf_impl_json_write_string_nocheck(
    struct wf_json_writer * writer,
    char const * value)
{
    size_t length = strlen(value);
    wf_impl_json_reserve(writer, length + WF_JSON_WRITER_ADDITIONAL_STRING_SIZE);
    wf_impl_json_begin_value(writer);

    wf_impl_json_write_raw_char(writer, '\"');
    wf_impl_json_write_raw(writer, value, length);
    wf_impl_json_write_raw_char(writer, '\"');

    wf_impl_json_end_value(writer);

}

void
wf_impl_json_write_bytes(
    struct wf_json_writer * writer,
    char const * data,
    size_t length)
{
    size_t encoded_length = wf_impl_base64_encoded_size(length);
    wf_impl_json_reserve(writer, length + WF_JSON_WRITER_ADDITIONAL_STRING_SIZE);
    wf_impl_json_begin_value(writer);

    wf_impl_json_write_raw_char(writer, '\"');
    wf_impl_base64_encode((uint8_t const *) data, length, &(writer->data[writer->offset]), encoded_length);
    writer->offset = encoded_length;
    wf_impl_json_write_raw_char(writer, '\"');

    wf_impl_json_end_value(writer);
}

void
wf_impl_json_write_array_begin(
    struct wf_json_writer * writer)
{
    wf_impl_json_reserve(writer, WF_JSON_WRITER_BEGIN_ARRAY_SIZE);
    wf_impl_json_begin_value(writer);
    wf_impl_json_push_state(writer, WF_JSON_WRITER_STATE_ARRAY_FIRST);
    wf_impl_json_write_raw_char(writer, '[');
}

void
wf_impl_json_write_array_end(
    struct wf_json_writer * writer)
{
    wf_impl_json_reserve(writer, WF_JSON_WRITER_END_ARRAY_SIZE);
    wf_impl_json_write_raw_char(writer, ']');
    wf_impl_json_pop_state(writer);
    wf_impl_json_end_value(writer);
}

void
wf_impl_json_write_object_begin(
    struct wf_json_writer * writer)
{
    wf_impl_json_reserve(writer, WF_JSON_WRITER_BEGIN_OBJECT_SIZE);
    wf_impl_json_begin_value(writer);
    wf_impl_json_push_state(writer, WF_JSON_WRITER_STATE_OBJECT_FIRST);
    wf_impl_json_write_raw_char(writer, '{');
}

void
wf_impl_json_write_object_end(
    struct wf_json_writer * writer)
{
    wf_impl_json_reserve(writer, WF_JSON_WRITER_END_OBJECT_SIZE);
    wf_impl_json_write_raw_char(writer, '}');
    wf_impl_json_pop_state(writer);
    wf_impl_json_end_value(writer);
}

void
wf_impl_json_write_object_key(
    struct wf_json_writer * writer,
    char const * key)
{
    size_t length = strlen(key);
    wf_impl_json_reserve(writer, length + WF_JSON_WRITER_ADDITIONAL_OBJECT_KEY_SIZE);

    if (WF_JSON_WRITER_STATE_OBJECT_NEXT == writer->state[writer->level])
    {
        wf_impl_json_write_raw_char(writer, ',');
    }
    else
    {
        writer->state[writer->level] = WF_JSON_WRITER_STATE_OBJECT_NEXT;
    }

    wf_impl_json_write_raw_char(writer, '\"');
    wf_impl_json_write_raw(writer, key, length);
    wf_impl_json_write_raw_char(writer, '\"');
    wf_impl_json_write_raw_char(writer, ':');
}

void
wf_impl_json_write_object_null(
    struct wf_json_writer * writer,
    char const * key)
{
    wf_impl_json_write_object_key(writer, key);
    wf_impl_json_write_null(writer);
}

void
wf_impl_json_write_object_bool(
    struct wf_json_writer * writer,
    char const * key,
    bool value)
{
    wf_impl_json_write_object_key(writer, key);
    wf_impl_json_write_bool(writer, value);
}

void
wf_impl_json_write_object_int(
    struct wf_json_writer * writer,
    char const * key,
    int value)
{
    wf_impl_json_write_object_key(writer, key);
    wf_impl_json_write_int(writer, value);
}

void
wf_impl_json_write_object_string(
    struct wf_json_writer * writer,
    char const * key,
    char const * value)
{
    wf_impl_json_write_object_key(writer, key);
    wf_impl_json_write_string(writer, value);
}

void
wf_impl_json_write_object_string_nocheck(
    struct wf_json_writer * writer,
    char const * key,
    char const * value)
{
    wf_impl_json_write_object_key(writer, key);
    wf_impl_json_write_string_nocheck(writer, value);
}

void
wf_impl_json_write_object_bytes(
    struct wf_json_writer * writer,
    char const * key,
    char const * data,
    size_t length)
{
    wf_impl_json_write_object_key(writer, key);
    wf_impl_json_write_bytes(writer, data, length);
}

void
wf_impl_json_write_object_begin_array(
    struct wf_json_writer * writer,
    char const * key)
{
    wf_impl_json_write_object_key(writer, key);
    wf_impl_json_write_array_begin(writer);
}

void
wf_impl_json_write_object_begin_object(
    struct wf_json_writer * writer,
    char const * key)
{
    wf_impl_json_write_object_key(writer, key);
    wf_impl_json_write_array_end(writer);
}

// -- 

static char
wf_impl_json_writer_get_esc(
    char c)
{
    switch (c)
    {
        case '\b': return 'b';
        case '\f': return 'f';
        case '\n': return 'n';
        case '\r': return 'r';
        case '\t': return 't';
        default:
            return c;
    }
}

static void
wf_impl_json_write_raw_char(
    struct wf_json_writer * writer,
    char value)
{
    writer->data[writer->offset++] = value;
}

static void
wf_impl_json_write_raw(
    struct wf_json_writer * writer,
    char const * value,
    size_t length)
{
    memcpy(&(writer->data[writer->offset]), value, length);
    writer->offset += length;
}

static void
wf_impl_json_reserve(
    struct wf_json_writer * writer,
    size_t needed)
{
    if ((writer->capacity - writer->offset) < needed)
    {
        size_t new_capacity = 2 * writer->capacity;
        while ((new_capacity - writer->offset) < needed)
        {
            new_capacity *= 2;
        }

        writer->raw_data = realloc(writer->raw_data, writer->pre + new_capacity);
        writer->data = &(writer->raw_data[writer->pre]);
        writer->capacity = new_capacity;
    }
}

static void
wf_impl_json_begin_value(
    struct wf_json_writer * writer)
{
    if (WF_JSON_WRITER_STATE_ARRAY_NEXT == writer->state[writer->level])
    {
        wf_impl_json_write_raw_char(writer, ',');
    }
}

static void
wf_impl_json_end_value(
    struct wf_json_writer * writer)
{
    if (WF_JSON_WRITER_STATE_ARRAY_FIRST == writer->state[writer->level])
    {
        writer->state[writer->level] = WF_JSON_WRITER_STATE_ARRAY_NEXT;
    }
}

static void
wf_impl_json_push_state(
    struct wf_json_writer * writer,
    enum wf_json_writer_state state)
{
    if (writer->level >= writer->max_level)
    {
        writer->max_level *= 2;
        writer->state = realloc(writer->state, (1 + writer->max_level) * sizeof(enum wf_json_writer_state));        
    }

    writer->level++;
    writer->state[writer->level] = state;
}

static void
wf_impl_json_pop_state(
    struct wf_json_writer * writer)
{
    if (writer->level > 0)
    {
        writer->level--;
    }
}
