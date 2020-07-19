#include "webfuse/impl/jsonrpc/request.h"
#include "webfuse/test_util/json_doc.hpp"
#include <gtest/gtest.h>

using webfuse_test::JsonDoc;

TEST(wf_jsonrpc_is_request, request_with_object_params)
{
    JsonDoc doc("{\"method\": \"method\", \"params\": {}, \"id\": 42}");

    ASSERT_TRUE(wf_impl_jsonrpc_is_request(doc.root()));
}

TEST(wf_jsonrpc_is_request, request_with_array_params)
{
    JsonDoc doc("{\"method\": \"method\", \"params\": [], \"id\": 42}");

    ASSERT_TRUE(wf_impl_jsonrpc_is_request(doc.root()));
}

TEST(wf_jsonrpc_is_request, null_request)
{
    ASSERT_FALSE(wf_impl_jsonrpc_is_request(nullptr));
}

TEST(wf_jsonrpc_is_request, invalid_request)
{
    JsonDoc doc("[\"method\", {}, 42]");

    ASSERT_FALSE(wf_impl_jsonrpc_is_request(doc.root()));
}

TEST(wf_jsonrpc_is_request, invalid_request_without_id)
{
    JsonDoc doc("{\"method\": \"method\", \"params\": {}}");

    ASSERT_FALSE(wf_impl_jsonrpc_is_request(doc.root()));
}

TEST(wf_jsonrpc_is_request, invalid_request_due_to_invalid_id)
{
    JsonDoc doc("{\"method\": \"method\", \"params\": [], \"id\": \"42\"}");

    ASSERT_FALSE(wf_impl_jsonrpc_is_request(doc.root()));
}

TEST(wf_jsonrpc_is_request, invalid_request_without_method)
{
    JsonDoc doc("{\"params\": [], \"id\": 42}");

    ASSERT_FALSE(wf_impl_jsonrpc_is_request(doc.root()));
}

TEST(wf_jsonrpc_is_request, invalid_request_due_to_invalid_method)
{
    JsonDoc doc("{\"method\": 42, \"params\": [], \"id\": 42}");

    ASSERT_FALSE(wf_impl_jsonrpc_is_request(doc.root()));
}

TEST(wf_jsonrpc_is_request, invalid_request_without_params)
{
    JsonDoc doc("{\"method\": \"method\", \"id\": 42}");

    ASSERT_FALSE(wf_impl_jsonrpc_is_request(doc.root()));
}

TEST(wf_jsonrpc_is_request, invalid_request_due_to_invalid_params)
{
    JsonDoc doc("{\"method\": \"method\", \"params\": \"params\", \"id\": 42}");

    ASSERT_FALSE(wf_impl_jsonrpc_is_request(doc.root()));
}
