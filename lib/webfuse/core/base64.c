#include "webfuse/core/base64.h"

static const uint8_t wf_base64_decode_table[256] = {
    // 0     1     2     3     4     5     6     7     8      9    A     B     C     D     E     F  
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 0
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 1
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,   62, 0x80, 0x80, 0x80,   63, // 2
      52,   53,   54,   55,   56,   57,   58,   59,   60,   61, 0x80, 0x80, 0x80,    0, 0x80, 0x80, // 3
    0x80,    0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,   12,   13,   14, // 4
      15,   16,   17,   18,   19,   20,   21,   22,   23,   24,   25, 0x80, 0x80, 0x80, 0x80, 0x80, // 5
    0x80,   26,   27,   28,   29,   30,   31,   32,   33,   34,   35,   36,   37,   38,   39,   40, // 6
      41,   42,   43,   44,   45,   46,   47,   48,   49,   50,   51, 0x80, 0x80, 0x80, 0x80, 0x80, // 7
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 8
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // 9
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // A
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // B
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // C
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // D
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // E
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, // F
};



size_t wf_base64_encoded_size(size_t length)
{
    return ((length + 2) / 3) * 4;
}

size_t wf_base64_encode(
    uint8_t const * data,
    size_t length,
    char * buffer,
    size_t buffer_size)
{    
    //                             0         1         2         3         4         5         6
    //                             0123456789012345678901234567890123456789012345678901234567890123
    static char const table[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t const length_needed = wf_base64_encoded_size(length);
    if (buffer_size < length_needed)
    {
        return 0;
    }

    size_t pos = 0;
    size_t out_pos = 0;
    for(; (length - pos) >= 3; pos += 3)
    {
        buffer[out_pos++] = table[ data[pos] >> 2 ];
        buffer[out_pos++] = table[ ((data[pos    ] & 0x03) << 4) | (data[pos + 1] >> 4) ];
        buffer[out_pos++] = table[ ((data[pos + 1] & 0x0f) << 2) | (data[pos + 2] >> 6) ];
        buffer[out_pos++] = table[ data[pos + 2] & 0x3f ];
    }

    switch((length - pos))
    {
        case 1:
            buffer[out_pos++] = table[ data[pos] >> 2 ];
            buffer[out_pos++] = table[ ((data[pos] & 0x03) << 4) ];
            buffer[out_pos++] = '=';
            buffer[out_pos++] = '=';
            break;
        case 2:
            buffer[out_pos++] = table[ data[pos] >> 2 ];
            buffer[out_pos++] = table[ ((data[pos    ] & 0x03) << 4) | (data[pos + 1] >> 4) ];
            buffer[out_pos++] = table[ ((data[pos + 1] & 0x0f) << 2) ];
            buffer[out_pos++] = '=';
            break;
        default:
            break;
    }

    if (buffer_size > out_pos)
    {
        buffer[out_pos] = '\0';
    }

    return out_pos;
}

size_t wf_base64_decoded_size(char const * data, size_t length)
{
    size_t result = 0;
    if ((length > 0) && ((length % 4) == 0))
    {
        result = (length / 4) * 3;

        if ('=' == data[length - 1])
        {
            result--;
            if ('=' == data[length - 2])
            {
                result--;
            }
        }
    }

    return result;
}

size_t wf_base64_decode(
    char const * data,
    size_t length,
    uint8_t * buffer,
    size_t buffer_size)
{
    uint8_t const * table = wf_base64_decode_table;
    size_t needed_size = wf_base64_decoded_size(data, length);
    if ((0 == needed_size) || (buffer_size < needed_size))
    {
        return 0;
    }

    size_t out_pos = 0;
    size_t pos = 0;
    for(; pos < length - 4; pos += 4)
    {
        uint8_t a = table[ (unsigned char) data[pos    ] ];
        uint8_t b = table[ (unsigned char) data[pos + 1] ];
        uint8_t c = table[ (unsigned char) data[pos + 2] ];
        uint8_t d = table[ (unsigned char) data[pos + 3] ];

        buffer[out_pos++] = (a << 2) | (b >> 4);
        buffer[out_pos++] = (b << 4) | (c >> 2);
        buffer[out_pos++] = (c << 6) | d;
    }

    // decode last block
    {
        uint8_t a = table[ (unsigned char) data[pos    ] ];
        uint8_t b = table[ (unsigned char) data[pos + 1] ];
        uint8_t c = table[ (unsigned char) data[pos + 2] ];
        uint8_t d = table[ (unsigned char) data[pos + 3] ];

        buffer[out_pos++] = (a << 2) | (b >> 4);
        if ('=' != data[pos + 2])
        {
            buffer[out_pos++] = (b << 4) | (c >> 2);
            if ('=' != data[pos + 3])
            {
                buffer[out_pos++] = (c << 6) | d;
            }
        }
    }

    return out_pos;
}

extern bool wf_base64_isvalid(char const * data, size_t length)
{
    uint8_t const * table = wf_base64_decode_table;

    if ((length == 0) || ((length % 4) != 0))
    {
        return false;
    }

    size_t pos = 0;
    for(; pos < (length - 2); pos++)
    {
        unsigned char c = (unsigned char) data[pos];
        if (('=' == c) || (0x80 == table[c]))
        {
            return false;
        }
    }

    if (('=' == data[pos]) && ('=' != data[pos + 1]))
    {
        return false;
    }

    for(;pos < length; pos++)
    {
        char c = data[pos];
        if (0x80 == table[ (unsigned char) c])
        {
            return false;
        }
    }

    return true;
}
