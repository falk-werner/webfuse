#include "webfuse/impl/json/node.h"
#include "webfuse/test_util/json_doc.hpp"

#include <gtest/gtest.h>

using webfuse_test::JsonDoc;

TEST(json_node, null)
{
    JsonDoc doc("null");
    ASSERT_EQ(WF_JSON_TYPE_NULL, wf_impl_json_type(doc.root()));
    ASSERT_TRUE(wf_impl_json_is_null(doc.root()));
}

TEST(json_node, true)
{
    JsonDoc doc("true");
    ASSERT_EQ(WF_JSON_TYPE_BOOL, wf_impl_json_type(doc.root()));
    ASSERT_TRUE(wf_impl_json_is_bool(doc.root()));
    ASSERT_TRUE(wf_impl_json_bool_get(doc.root()));
}

TEST(json_node, false)
{
    JsonDoc doc("false");
    ASSERT_EQ(WF_JSON_TYPE_BOOL, wf_impl_json_type(doc.root()));
    ASSERT_TRUE(wf_impl_json_is_bool(doc.root()));
    ASSERT_FALSE(wf_impl_json_bool_get(doc.root()));
}

TEST(json_node, int)
{
    JsonDoc doc("42");
    ASSERT_EQ(WF_JSON_TYPE_INT, wf_impl_json_type(doc.root()));
    ASSERT_TRUE(wf_impl_json_is_int(doc.root()));
    ASSERT_EQ(42, wf_impl_json_int_get(doc.root()));
}

TEST(json_node, string)
{
    JsonDoc doc("\"brummni\"");
    ASSERT_EQ(WF_JSON_TYPE_STRING, wf_impl_json_type(doc.root()));
    ASSERT_TRUE(wf_impl_json_is_string(doc.root()));
    ASSERT_STREQ("brummni", wf_impl_json_string_get(doc.root()));
}

TEST(json_node, array)
{
    JsonDoc doc("[1,2,3]");
    ASSERT_EQ(WF_JSON_TYPE_ARRAY, wf_impl_json_type(doc.root()));
    ASSERT_TRUE(wf_impl_json_is_array(doc.root()));
    ASSERT_EQ(3, wf_impl_json_array_size(doc.root()));
    ASSERT_EQ(WF_JSON_TYPE_INT, wf_impl_json_type(wf_impl_json_array_get(doc.root(), 0)));
    ASSERT_EQ(WF_JSON_TYPE_UNDEFINED, wf_impl_json_type(wf_impl_json_array_get(doc.root(), 4)));
}

TEST(json_node, object)
{
    JsonDoc doc("{\"answer\": 42}");
    ASSERT_EQ(WF_JSON_TYPE_OBJECT, wf_impl_json_type(doc.root()));
    ASSERT_TRUE(wf_impl_json_is_object(doc.root()));
    ASSERT_EQ(1, wf_impl_json_object_size(doc.root()));
    
    ASSERT_EQ(WF_JSON_TYPE_INT, wf_impl_json_type(wf_impl_json_object_get(doc.root(), "answer")));
    ASSERT_STREQ("answer", wf_impl_json_object_key(doc.root(), 0));
    ASSERT_EQ(WF_JSON_TYPE_INT, wf_impl_json_type(wf_impl_json_object_value(doc.root(), 0)));

    ASSERT_EQ(WF_JSON_TYPE_UNDEFINED, wf_impl_json_type(wf_impl_json_object_get(doc.root(), "unknown")));
    ASSERT_STREQ("", wf_impl_json_object_key(doc.root(), 1));
    ASSERT_EQ(WF_JSON_TYPE_UNDEFINED, wf_impl_json_type(wf_impl_json_object_value(doc.root(), 1)));
}

TEST(json_node, default_values)
{
    JsonDoc doc("null");

    ASSERT_EQ   (false, wf_impl_json_bool_get(doc.root()));
    ASSERT_EQ   (0    , wf_impl_json_int_get(doc.root()));
    ASSERT_STREQ(""   , wf_impl_json_string_get(doc.root()));

    ASSERT_EQ   (0    , wf_impl_json_array_size(doc.root()));
    ASSERT_EQ   (WF_JSON_TYPE_UNDEFINED, wf_impl_json_type(wf_impl_json_array_get(doc.root(), 0)));

    ASSERT_EQ   (0    , wf_impl_json_object_size(doc.root()));
    ASSERT_STREQ(""   , wf_impl_json_object_key(doc.root(), 0));
    ASSERT_EQ   (WF_JSON_TYPE_UNDEFINED, wf_impl_json_type(wf_impl_json_object_get(doc.root(), 0)));
    ASSERT_EQ   (WF_JSON_TYPE_UNDEFINED, wf_impl_json_type(wf_impl_json_object_value(doc.root(), 0)));

}