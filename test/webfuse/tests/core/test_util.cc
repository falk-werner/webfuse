#include <gtest/gtest.h>
#include "webfuse/core/json_util.h"

TEST(jsonrpc_util, get_int)
{
    json_t * object = json_object();
    json_object_set_new(object, "key", json_integer(23));
    int value = wf_impl_json_get_int(object, "key", 42);
    ASSERT_EQ(23, value);

    json_decref(object);
}

TEST(jsonrpc_util, failed_to_get_null_object)
{
    int value = wf_impl_json_get_int(nullptr, "key", 42);
    
    ASSERT_EQ(42, value);
}

TEST(jsonrpc_util, failed_to_get_not_object)
{
    json_t * object = json_array();
    int value = wf_impl_json_get_int(nullptr, "key", 42);    
    ASSERT_EQ(42, value);

    json_decref(object);
}

TEST(jsonrpc_util, failed_to_get_invalid_key)
{
    json_t * object = json_object();
    int value = wf_impl_json_get_int(object, "key", 42);
    ASSERT_EQ(42, value);

    json_decref(object);
}

TEST(jsonrpc_util, failed_to_get_invalid_value_type)
{
    json_t * object = json_object();
    json_object_set_new(object, "key", json_string("42"));
    int value = wf_impl_json_get_int(object, "key", 42);
    ASSERT_EQ(42, value);

    json_decref(object);
}