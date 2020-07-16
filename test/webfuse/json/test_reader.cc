#include "webfuse/impl/json/reader.h"
#include <gtest/gtest.h>
#include <climits>

TEST(json_reader, skip_whitespace)
{
    char text[] = " \t\r\n*";
    wf_json_reader reader;
    wf_impl_json_reader_init(&reader, text, 5);
    char c = wf_impl_json_reader_skip_whitespace(&reader);
    ASSERT_EQ('*', c);
    ASSERT_STREQ("*", &reader.contents[reader.pos]);
}

TEST(json_reader, skip_whitespace_eof)
{
    char text[] = " ";
    wf_json_reader reader;
    wf_impl_json_reader_init(&reader, text, 1);
    char c = wf_impl_json_reader_skip_whitespace(&reader);
    ASSERT_EQ('\0', c);
}

TEST(json_reader, peek)
{
    char text[] = "*";
    wf_json_reader reader;
    wf_impl_json_reader_init(&reader, text, 1);
    char c = wf_impl_json_reader_peek(&reader);
    ASSERT_EQ('*', c);
}

TEST(json_reader, peek_eof)
{
    wf_json_reader reader;
    wf_impl_json_reader_init(&reader, nullptr, 0);
    char c = wf_impl_json_reader_peek(&reader);
    ASSERT_EQ('\0', c);
}

TEST(json_reader, get_char)
{
    char text[] = "*";
    wf_json_reader reader;
    wf_impl_json_reader_init(&reader, text, 1);
    ASSERT_EQ('*', wf_impl_json_reader_get_char(&reader));
    ASSERT_EQ('\0', wf_impl_json_reader_get_char(&reader));
    ASSERT_EQ('\0', wf_impl_json_reader_get_char(&reader));
}

TEST(json_reader, unget_char)
{
    char text[] = "*";
    wf_json_reader reader;
    wf_impl_json_reader_init(&reader, text, 1);

    ASSERT_EQ('*', wf_impl_json_reader_get_char(&reader));
    wf_impl_json_reader_unget_char(&reader);
    ASSERT_EQ('*', wf_impl_json_reader_get_char(&reader));

    wf_impl_json_reader_unget_char(&reader);
    wf_impl_json_reader_unget_char(&reader);
    ASSERT_EQ('*', wf_impl_json_reader_get_char(&reader));
}

TEST(json_reader, read_const)
{
    char text[] = "value";
    wf_json_reader reader;
    wf_impl_json_reader_init(&reader, text, 5);

    ASSERT_TRUE(wf_impl_json_reader_read_const(&reader, "value", 5));
}

TEST(json_reader, read_const_fail_out_of_bounds)
{
    char text[] = "value";
    wf_json_reader reader;
    wf_impl_json_reader_init(&reader, text, 2);

    ASSERT_FALSE(wf_impl_json_reader_read_const(&reader, "value", 5));
}

TEST(json_reader, read_const_fail_no_match)
{
    char text[] = "hello";
    wf_json_reader reader;
    wf_impl_json_reader_init(&reader, text, 5);

    ASSERT_FALSE(wf_impl_json_reader_read_const(&reader, "value", 5));
}

TEST(json_reader, read_positive_int)
{
    char text[] = "42";
    wf_json_reader reader;
    wf_impl_json_reader_init(&reader, text, 5);

    int value;
    ASSERT_TRUE(wf_impl_json_reader_read_int(&reader, &value));
    ASSERT_EQ(42, value);
}

TEST(json_reader, read_negative_int)
{
    char text[] = "-1234";
    wf_json_reader reader;
    wf_impl_json_reader_init(&reader, text, 5);

    int value;
    ASSERT_TRUE(wf_impl_json_reader_read_int(&reader, &value));
    ASSERT_EQ(-1234, value);
}

TEST(json_reader, read_int_max)
{
    std::string text = std::to_string(INT_MAX);
    wf_json_reader reader;
    wf_impl_json_reader_init(&reader, const_cast<char*>(text.data()), text.size());

    int value;
    ASSERT_TRUE(wf_impl_json_reader_read_int(&reader, &value));
    ASSERT_EQ(INT_MAX, value);
}

TEST(json_reader, read_int_min)
{
    std::string text = std::to_string(INT_MIN);
    wf_json_reader reader;
    wf_impl_json_reader_init(&reader, const_cast<char*>(text.data()), text.size());

    int value;
    ASSERT_TRUE(wf_impl_json_reader_read_int(&reader, &value));
    ASSERT_EQ(INT_MIN, value);
}

TEST(json_reader, read_int_fail_invalid)
{
    std::string text = "brummni";
    wf_json_reader reader;
    wf_impl_json_reader_init(&reader, const_cast<char*>(text.data()), text.size());

    int value;
    ASSERT_FALSE(wf_impl_json_reader_read_int(&reader, &value));
}

TEST(json_reader, read_int_fail_sign_only)
{
    std::string text = "-";
    wf_json_reader reader;
    wf_impl_json_reader_init(&reader, const_cast<char*>(text.data()), text.size());

    int value;
    ASSERT_FALSE(wf_impl_json_reader_read_int(&reader, &value));
}

TEST(json_reader, read_string)
{
    std::string text = "\"brummni\"";
    wf_json_reader reader;
    wf_impl_json_reader_init(&reader, const_cast<char*>(text.data()), text.size());

    char * value;
    size_t size;
    ASSERT_TRUE(wf_impl_json_reader_read_string(&reader, &value, &size));
    ASSERT_STREQ("brummni", value);
    ASSERT_EQ(7, size);
}

TEST(json_reader, read_string_escaped)
{
    std::string text = "\"_\\\"_\\\\_\\/_\\b_\\f_\\n_\\r_\\t_\"";
    wf_json_reader reader;
    wf_impl_json_reader_init(&reader, const_cast<char*>(text.data()), text.size());

    char * value;
    size_t size;
    ASSERT_TRUE(wf_impl_json_reader_read_string(&reader, &value, &size));
    ASSERT_STREQ("_\"_\\_/_\b_\f_\n_\r_\t_", value);
    ASSERT_EQ(17, size);
}

TEST(json_reader, read_string_fail_missig_start_quot)
{
    std::string text = "brummni\"";
    wf_json_reader reader;
    wf_impl_json_reader_init(&reader, const_cast<char*>(text.data()), text.size());

    char * value;
    size_t size;
    ASSERT_FALSE(wf_impl_json_reader_read_string(&reader, &value, &size));
}

TEST(json_reader, read_string_fail_missig_end_quot)
{
    std::string text = "\"brummni";
    wf_json_reader reader;
    wf_impl_json_reader_init(&reader, const_cast<char*>(text.data()), text.size());

    char * value;
    size_t size;
    ASSERT_FALSE(wf_impl_json_reader_read_string(&reader, &value, &size));
}

TEST(json_reader, read_string_fail_invalid_escape_seq)
{
    std::string text = "\"\\i\"";
    wf_json_reader reader;
    wf_impl_json_reader_init(&reader, const_cast<char*>(text.data()), text.size());

    char * value;
    size_t size;
    ASSERT_FALSE(wf_impl_json_reader_read_string(&reader, &value, &size));
}
