#include "webfuse/impl/json/parser.h"
#include "webfuse/impl/json/reader.h"
#include "webfuse/impl/json/node_intern.h"

#include <stdlib.h>

#define WF_JSON_PARSER_INITIAL_CAPACITY 4

static bool
wf_impl_json_parse_null(
    struct wf_json_reader * reader,
    struct wf_json * json);

static bool
wf_impl_json_parse_true(
    struct wf_json_reader * reader,
    struct wf_json * json);

static bool
wf_impl_json_parse_false(
    struct wf_json_reader * reader,
    struct wf_json * json);

static bool
wf_impl_json_parse_int(
    struct wf_json_reader * reader,
    struct wf_json * json);

static bool
wf_impl_json_parse_string(
    struct wf_json_reader * reader,
    struct wf_json * json);

static bool
wf_impl_json_parse_array(
    struct wf_json_reader * reader,
    struct wf_json * json);

static bool
wf_impl_json_parse_object(
    struct wf_json_reader * reader,
    struct wf_json * json);

// --

bool
wf_impl_json_parse_value(
    struct wf_json_reader * reader,
    struct wf_json * json)
{
    bool result = false;
    char const c = wf_impl_json_reader_skip_whitespace(reader);

    switch (c)
    {
        case 'n':
            result = wf_impl_json_parse_null(reader, json);
            break;
        case 't':
            result = wf_impl_json_parse_true(reader, json);
            break;
        case 'f':
            result = wf_impl_json_parse_false(reader, json);
            break;
        case '\"':
            result = wf_impl_json_parse_string(reader, json);
            break;
        case '[':
            result = wf_impl_json_parse_array(reader, json);
            break;
        case '{':
            result = wf_impl_json_parse_object(reader, json);
            break;
        case '-': // fall-through
        case '0': // fall-through
        case '1': // fall-through
        case '2': // fall-through
        case '3': // fall-through
        case '4': // fall-through
        case '5': // fall-through
        case '6': // fall-through
        case '7': // fall-through
        case '8': // fall-through
        case '9':
            result = wf_impl_json_parse_int(reader, json);
            break;
        default:
            result = false;
            break;
    }

    return result;
}

// --

static bool
wf_impl_json_parse_null(
    struct wf_json_reader * reader,
    struct wf_json * json)
{
    bool const result = wf_impl_json_reader_read_const(reader, "null", 4);
    if (result)
    {
        json->type = WF_JSON_TYPE_NULL;
    }

    return result;
}

static bool
wf_impl_json_parse_true(
    struct wf_json_reader * reader,
    struct wf_json * json)
{
    bool const result = wf_impl_json_reader_read_const(reader, "true", 4);
    if (result)
    {
        json->type = WF_JSON_TYPE_BOOL;
        json->value.b = true;
    }

    return result;
}

static bool
wf_impl_json_parse_false(
    struct wf_json_reader * reader,
    struct wf_json * json)
{
    bool const result = wf_impl_json_reader_read_const(reader, "false", 5);
    if (result)
    {
        json->type = WF_JSON_TYPE_BOOL;
        json->value.b = false;
    }

    return result;
}


static bool
wf_impl_json_parse_int(
    struct wf_json_reader * reader,
    struct wf_json * json)
{
    int value;
    bool const result = wf_impl_json_reader_read_int(reader, &value);
    if (result)
    {
        json->type = WF_JSON_TYPE_INT;
        json->value.i = value;
    }

    return result;
}

static bool
wf_impl_json_parse_string(
    struct wf_json_reader * reader,
    struct wf_json * json)
{
    char * value;
    bool const result = wf_impl_json_reader_read_string(reader, &value);
    if (result)
    {
        json->type = WF_JSON_TYPE_STRING;
        json->value.s = value;
    }

    return result;
}

static bool
wf_impl_json_parse_array(
    struct wf_json_reader * reader,
    struct wf_json * json)
{
    wf_impl_json_reader_skip_whitespace(reader);
    char c = wf_impl_json_reader_get_char(reader);
    if ('[' != c) { return false; }

    size_t capacity = WF_JSON_PARSER_INITIAL_CAPACITY;
    json->type = WF_JSON_TYPE_ARRAY;
    json->value.a.items = malloc(sizeof(struct wf_json) * capacity);
    json->value.a.size = 0;

    c = wf_impl_json_reader_skip_whitespace(reader);
    if (']' == c)
    {
        wf_impl_json_reader_get_char(reader);
        return true;
    }

    bool result;
    do
    {
        if (json->value.a.size >= capacity)
        {
            capacity *= 2;
            json->value.a.items = realloc(json->value.a.items, sizeof(struct wf_json) * capacity);
        }

        result = wf_impl_json_parse_value(reader, &(json->value.a.items[json->value.a.size]));
        if (result)
        {
            json->value.a.size++;
            wf_impl_json_reader_skip_whitespace(reader);
            c = wf_impl_json_reader_get_char(reader);
        }
    } while ((result) && (',' == c));
    
    if ((result) && (']' != c))
    {
        result = false;
    }

    if (!result)
    {
        wf_impl_json_cleanup(json);
    }

    return result;
}

static bool
wf_impl_json_parse_object(
    struct wf_json_reader * reader,
    struct wf_json * json)
{
    wf_impl_json_reader_skip_whitespace(reader);
    char c = wf_impl_json_reader_get_char(reader);
    if ('{' != c) { return false; }

    size_t capacity = WF_JSON_PARSER_INITIAL_CAPACITY;
    json->type = WF_JSON_TYPE_OBJECT;
    json->value.o.items = malloc(sizeof(struct wf_json_object_item) * capacity);
    json->value.o.size = 0;

    c = wf_impl_json_reader_skip_whitespace(reader);
    if ('}' == c)
    {
        wf_impl_json_reader_get_char(reader);
        return true;
    }

    bool result;
    do
    {
        if (json->value.o.size >= capacity)
        {
            capacity *= 2;
            json->value.o.items = realloc(json->value.o.items, sizeof(struct wf_json_object_item) * capacity);
        }

        struct wf_json_object_item * item = &(json->value.o.items[json->value.o.size]);
        result = wf_impl_json_reader_read_string(reader, &(item->key));
        if (result)
        {
            wf_impl_json_reader_skip_whitespace(reader);
            result = (':' == wf_impl_json_reader_get_char(reader));
        }

        if (result)
        {
            result = wf_impl_json_parse_value(reader, &(item->json));
        }

        if (result)
        {
            json->value.o.size++;
            wf_impl_json_reader_skip_whitespace(reader);
            c = wf_impl_json_reader_get_char(reader);
        }
    } while ((reader) && (',' == c));

    if ((reader) && ('}' != c))
    {
        result = false;
    }

    if (!result)
    {
        wf_impl_json_cleanup(json);
    }

    return result;
}