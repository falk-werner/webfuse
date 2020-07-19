#include "webfuse/impl/util/json_util.h"
#include "webfuse/test_util/json_doc.hpp"

#include <gtest/gtest.h>

using webfuse_test::JsonDoc;

TEST(jsonrpc_util, get_int)
{
    JsonDoc doc("{\"key\": 23}");
    int value = wf_impl_json_get_int(doc.root(), "key", 42);
    ASSERT_EQ(23, value);
}

TEST(jsonrpc_util, failed_to_get_null_object)
{
    int value = wf_impl_json_get_int(nullptr, "key", 42);
    
    ASSERT_EQ(42, value);
}

TEST(jsonrpc_util, failed_to_get_not_object)
{
    JsonDoc doc("[]");
    int value = wf_impl_json_get_int(doc.root(), "key", 42);    
    ASSERT_EQ(42, value);
}

TEST(jsonrpc_util, failed_to_get_invalid_key)
{
    JsonDoc doc("{}");
    int value = wf_impl_json_get_int(doc.root(), "key", 42);
    ASSERT_EQ(42, value);
}

TEST(jsonrpc_util, failed_to_get_invalid_value_type)
{
    JsonDoc doc("{\"key\": \"42\"}");
    int value = wf_impl_json_get_int(doc.root(), "key", 42);
    ASSERT_EQ(42, value);
}