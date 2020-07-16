#include "webfuse/impl/json/reader.h"

#include <string.h>

static char
wf_impl_json_unescape(
    char c);

void
wf_impl_json_reader_init(
    struct wf_json_reader * reader,
    char * contents,
    size_t length)
{
    reader->contents = contents;
    reader->length = length;
    reader->pos = 0;
}

char
wf_impl_json_reader_skip_whitespace(
    struct wf_json_reader * reader)
{
    char c = wf_impl_json_reader_peek(reader);
    while ((' ' == c) || ('\n' == c) || ('\t' == c) || ('\r' == c))
    {
        reader->pos++;
        c = wf_impl_json_reader_peek(reader);
    }

    return c;
}

char
wf_impl_json_reader_peek(
    struct wf_json_reader * reader)
{
    char result = '\0';
    if (reader->pos < reader->length)
    {
        result = reader->contents[reader->pos];
    }

    return result;
}

char
wf_impl_json_reader_get_char(
    struct wf_json_reader * reader)
{
    char result = '\0';
    if (reader->pos < reader->length)
    {
        result = reader->contents[reader->pos++];
    }

    return result;
}

void
wf_impl_json_reader_unget_char(
    struct wf_json_reader * reader)
{
    if (0 < reader->pos)
    {
        reader->pos--;
    }
}

bool
wf_impl_json_reader_read_const(
    struct wf_json_reader * reader,
    char const * value,
    size_t length)
{
    size_t remaining = reader->length - reader->pos;
    bool const result = ((remaining >= length) && (0 == strncmp((&reader->contents[reader->pos]), value, length)));
    if (result)
    {
        reader->pos += length;
    }

    return result;
}

bool
wf_impl_json_reader_read_int(
    struct wf_json_reader * reader,
    int * value)
{
    char c = wf_impl_json_reader_get_char(reader);
    bool const is_signed = ('-' == c);
    if (is_signed)
    {
        c = wf_impl_json_reader_get_char(reader);
    }

    bool const result = (('0' <= c) && (c <= '9'));
    if (result)
    {
        int v = c - '0';
        c = wf_impl_json_reader_peek(reader);
        while (('0' <= c) && (c <= '9'))
        {
            v *= 10;
            v += c - '0';
            reader->pos++;
            c = wf_impl_json_reader_peek(reader);
        }

        *value = (is_signed) ? -v : v;
    }

    return result;
}

bool
wf_impl_json_reader_read_string(
    struct wf_json_reader * reader,
    char * * value,
    size_t * size)
{
    wf_impl_json_reader_skip_whitespace(reader);
    char c = wf_impl_json_reader_get_char(reader);
    if ('\"' != c) { return false; }

    size_t start = reader->pos;
    size_t p = reader->pos;
    c = wf_impl_json_reader_get_char(reader);
    while (('\"' != c) && ('\0' != c))
    {
        if ('\\' != c)
        {
            reader->contents[p++] = c;
        }
        else
        {
            char unescaped = wf_impl_json_unescape(wf_impl_json_reader_get_char(reader));
            if ('\0' != unescaped)
            {
                reader->contents[p++] = unescaped;

            }
            else
            {
                return false;
            }
        }

        c = wf_impl_json_reader_get_char(reader);
    }

    bool const result = ('\"' == c);
    if (result)
    {
        reader->contents[p] = '\0';
        *value = &(reader->contents[start]);
        *size = p - start;
    }

    return result;
}

static char
wf_impl_json_unescape(
    char c)
{
    switch (c)
    {
        case '\"': return '\"';
        case '\\': return '\\';
        case '/': return '/';
        case 'b': return '\b';
        case 'f': return '\f';
        case 'n': return '\n';
        case 'r': return '\r';
        case 't': return '\t';
        default:
            return '\0';
    }
}
