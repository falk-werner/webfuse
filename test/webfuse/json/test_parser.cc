#include "webfuse/impl/json/parser.h"
#include "webfuse/impl/json/reader.h"
#include "webfuse/impl/json/node_intern.h"

#include <gtest/gtest.h>
#include <string>

namespace
{

bool try_parse(std::string const & value)
{
    std::string contents = value;
    struct wf_json_reader reader;
    wf_impl_json_reader_init(&reader, const_cast<char*>(contents.data()), contents.size());
    wf_json json;

    bool const result = wf_impl_json_parse_value(&reader, &json);
    if (result)
    {
        wf_impl_json_cleanup(&json);
    }

    return result;
}

}


TEST(json_parser, fail_no_contents)
{
    ASSERT_FALSE(try_parse(""));
}

TEST(json_parser, fail_invalid_null)
{
    ASSERT_FALSE(try_parse("none"));
}

TEST(json_parser, fail_invalid_true)
{
    ASSERT_FALSE(try_parse("tru"));
}

TEST(json_parser, fail_invalid_false)
{
    ASSERT_FALSE(try_parse("fals"));
}

TEST(json_parser, fail_invalid_int)
{
    ASSERT_FALSE(try_parse("-"));
}

TEST(json_parser, fail_invalid_string)
{
    ASSERT_FALSE(try_parse("\"invalid"));
}

TEST(json_parser, empty_array)
{
    ASSERT_TRUE(try_parse("[]"));
}

TEST(json_parser, large_array)
{
    ASSERT_TRUE(try_parse("[1,2,3,4,5,6,7,8,9]"));
}

TEST(json_parser, fail_unterminated_array)
{
    ASSERT_FALSE(try_parse("[1"));
}

TEST(json_parser, fail_missing_array_separator)
{
    ASSERT_FALSE(try_parse("[1 2]"));
}

TEST(json_parser, fail_missing_array_value)
{
    ASSERT_FALSE(try_parse("[1,]"));
}

TEST(json_parser, empty_object)
{
    ASSERT_TRUE(try_parse("{}"));
}

TEST(json_parser, large_object)
{
    ASSERT_TRUE(try_parse("{\"a\":1,\"b\":2,\"c\":3,\"d\":4,\"e\":5}"));
}

TEST(json_parser, fail_unterminated_object)
{
    ASSERT_FALSE(try_parse("{\"a\":1"));
}

TEST(json_parser, fail_invalid_object_key)
{
    ASSERT_FALSE(try_parse("{a:1}"));
}

TEST(json_parser, fail_missing_object_terminator)
{
    ASSERT_FALSE(try_parse("{\"a\"1}"));
}

TEST(json_parser, fail_missing_object_value)
{
    ASSERT_FALSE(try_parse("{\"a\":}"));
}

TEST(json_parser, fail_missing_object_separator)
{
    ASSERT_FALSE(try_parse("{\"a\":1 \"b\":2}"));
}

TEST(json_parser, fail_missing_object_item)
{
    ASSERT_FALSE(try_parse("{\"a\":1,}"));
}
