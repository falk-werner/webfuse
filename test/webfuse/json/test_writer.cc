#include "webfuse/impl/json/writer.h"
#include <gtest/gtest.h>
#include <cstdlib>
#include <climits>

namespace
{

class writer
{
public:
    writer(size_t initial_capacity = 128)
    : writer_(wf_impl_json_writer_create(initial_capacity, 0))
    {
    }

    ~writer()
    {
        wf_impl_json_writer_dispose(writer_);
    }

    operator  wf_json_writer * ()
    {
        return writer_;
    }

    std::string take()
    {
        char * text = wf_impl_json_writer_take(writer_, nullptr);
        std::string result = text;
        free(text);

        return result;
    } 

private:
    wf_json_writer * writer_;
};

}

TEST(json_writer, write_null)
{
    writer writer;
    
    wf_impl_json_write_null(writer);
    ASSERT_STREQ("null", writer.take().c_str());
}

TEST(json_writer, write_true)
{
    writer writer;
    
    wf_impl_json_write_bool(writer, true);
    ASSERT_STREQ("true", writer.take().c_str());
}

TEST(json_writer, write_false)
{
    writer writer;
    
    wf_impl_json_write_bool(writer, false);
    ASSERT_STREQ("false", writer.take().c_str());
}

TEST(json_writer, positive_int)
{
    writer writer;
    
    wf_impl_json_write_int(writer, 42);
    ASSERT_STREQ("42", writer.take().c_str());
}

TEST(json_writer, int_max)
{
    writer writer;
    std::string int_max = std::to_string(INT_MAX);

    wf_impl_json_write_int(writer, INT_MAX);
    ASSERT_EQ(int_max, writer.take());
}

TEST(json_writer, negative_int)
{
    writer writer;
    
    wf_impl_json_write_int(writer, -12345);
    ASSERT_STREQ("-12345", writer.take().c_str());
}

TEST(json_writer, int_min)
{
    writer writer;
    std::string int_min = std::to_string(INT_MIN);

    wf_impl_json_write_int(writer, INT_MIN);
    ASSERT_EQ(int_min, writer.take());
}

TEST(json_writer, write_string)
{
    writer writer;
    
    wf_impl_json_write_string(writer, "brummni");
    ASSERT_STREQ("\"brummni\"", writer.take().c_str());
}

TEST(json_writer, write_string_escape)
{
    writer writer;
    
    wf_impl_json_write_string(writer, "_\"_\\_\r_\n_\t_\b_\f_");
    ASSERT_STREQ("\"_\\\"_\\\\_\\r_\\n_\\t_\\b_\\f_\"", writer.take().c_str());
}

TEST(json_writer, write_string_nocheck)
{
    writer writer;
    
    wf_impl_json_write_string_nocheck(writer, "_\"_\\_\r_\n_\t_\b_\f_");
    ASSERT_STREQ("\"_\"_\\_\r_\n_\t_\b_\f_\"", writer.take().c_str());
}

TEST(json_writer, write_empty_array)
{
    writer writer;
    
    wf_impl_json_write_array_begin(writer);
    wf_impl_json_write_array_end(writer);
    ASSERT_STREQ("[]", writer.take().c_str());
}

TEST(json_writer, write_complex_array)
{
    writer writer;
    
    wf_impl_json_write_array_begin(writer);
    wf_impl_json_write_null(writer);
    wf_impl_json_write_bool(writer, true);

    wf_impl_json_write_array_begin(writer);
    wf_impl_json_write_int(writer, 0);
    wf_impl_json_write_string(writer, "foo");
    wf_impl_json_write_array_begin(writer);

    wf_impl_json_write_array_end(writer);
    wf_impl_json_write_array_end(writer);
    wf_impl_json_write_array_end(writer);

    ASSERT_STREQ("[null,true,[0,\"foo\",[]]]", writer.take().c_str());
}

TEST(json_writer, write_empty_object)
{
    writer writer;
    
    wf_impl_json_write_object_begin(writer);
    wf_impl_json_write_object_end(writer);
    ASSERT_STREQ("{}", writer.take().c_str());
}

TEST(json_writer, write_comple_object)
{
    writer writer;
    
    wf_impl_json_write_object_begin(writer);
    wf_impl_json_write_object_string(writer, "method", "add");
    wf_impl_json_write_object_begin_array(writer, "params");
    wf_impl_json_write_int(writer, 1);
    wf_impl_json_write_int(writer, 2);
    wf_impl_json_write_array_end(writer);
    wf_impl_json_write_object_int(writer, "id", 42);
    wf_impl_json_write_object_end(writer);
    ASSERT_STREQ("{\"method\":\"add\",\"params\":[1,2],\"id\":42}", writer.take().c_str());
}

TEST(json_writer, write_deep_nested_array)
{
    writer writer;
    size_t count = 10;
    std::string expected = "";

    for (size_t i = 0; i < count; i++)
    {
        wf_impl_json_write_array_begin(writer);
        expected += "[";
    }

    for (size_t i = 0; i < count; i++)
    {
        wf_impl_json_write_array_end(writer);        
        expected += "]";
    }

    ASSERT_EQ(expected, writer.take());
}

TEST(json_writer, write_bytes)
{
    writer writer;
    wf_impl_json_write_bytes(writer, "\0\0", 2);

    ASSERT_EQ("\"AAA\"", writer.take());
}

TEST(json_writer, reset)
{
    writer writer;
    wf_impl_json_write_string(writer, "some value");

    wf_impl_json_writer_reset(writer);
    wf_impl_json_write_int(writer,42);

    ASSERT_EQ("42", writer.take());
}

TEST(json_writer, write_object_null)
{
    writer writer;
    wf_impl_json_write_object_begin(writer);
    wf_impl_json_write_object_null(writer, "error");
    wf_impl_json_write_object_end(writer);

    ASSERT_EQ("{\"error\":null}", writer.take());
}

TEST(json_writer, write_object_bool)
{
    writer writer;
    wf_impl_json_write_object_begin(writer);
    wf_impl_json_write_object_bool(writer, "result", true);
    wf_impl_json_write_object_end(writer);

    ASSERT_EQ("{\"result\":true}", writer.take());
}

TEST(json_writer, write_object_string_nocheck)
{
    writer writer;
    wf_impl_json_write_object_begin(writer);
    wf_impl_json_write_object_string(writer, "result", "Hello,\tWorld!");
    wf_impl_json_write_object_end(writer);

    ASSERT_EQ("{\"result\": \"Hello,\tWorld!\"}", writer.take());
}

TEST(json_writer, write_object_bytes)
{
    writer writer;
    wf_impl_json_write_object_begin(writer);
    wf_impl_json_write_object_bytes(writer, "result", "\0\0", 2);
    wf_impl_json_write_object_end(writer);

    ASSERT_EQ("{\"result\": \"AAA\"}", writer.take());
}

TEST(json_writer, realloc_buffer)
{
    writer writer(1);
    wf_impl_json_write_string(writer, "very large contents");

    ASSERT_EQ("very large contents", writer.take());
}

TEST(json_writer, unexpected_end)
{
    writer writer;
    wf_impl_json_write_array_end(writer);
}
