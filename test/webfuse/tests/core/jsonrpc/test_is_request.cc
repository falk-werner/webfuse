#include <gtest/gtest.h>
#include "webfuse/core/jsonrpc/request.h"

TEST(wf_jsonrpc_is_request, request_with_object_params)
{
    json_t * request = json_object();
    json_object_set_new(request, "method", json_string("method"));
    json_object_set_new(request, "params", json_object());
    json_object_set_new(request, "id", json_integer(42));

    ASSERT_TRUE(wf_jsonrpc_is_request(request));

    json_decref(request);
}

TEST(wf_jsonrpc_is_request, request_with_array_params)
{
    json_t * request = json_object();
    json_object_set_new(request, "method", json_string("method"));
    json_object_set_new(request, "params", json_array());
    json_object_set_new(request, "id", json_integer(42));

    ASSERT_TRUE(wf_jsonrpc_is_request(request));

    json_decref(request);
}

TEST(wf_jsonrpc_is_request, null_request)
{
    ASSERT_FALSE(wf_jsonrpc_is_request(nullptr));
}

TEST(wf_jsonrpc_is_request, invalid_request)
{
    json_t * request = json_array();
    json_array_append_new(request, json_string("method"));
    json_array_append_new(request, json_object());
    json_array_append_new(request, json_integer(42));

    ASSERT_FALSE(wf_jsonrpc_is_request(request));

    json_decref(request);
}

TEST(wf_jsonrpc_is_request, invalid_request_without_id)
{
    json_t * request = json_object();
    json_object_set_new(request, "method", json_string("method"));
    json_object_set_new(request, "params", json_object());

    ASSERT_FALSE(wf_jsonrpc_is_request(request));

    json_decref(request);
}

TEST(wf_jsonrpc_is_request, invalid_request_due_to_invalid_id)
{
    json_t * request = json_object();
    json_object_set_new(request, "method", json_string("method"));
    json_object_set_new(request, "params", json_object());
    json_object_set_new(request, "id", json_string("42"));

    ASSERT_FALSE(wf_jsonrpc_is_request(request));

    json_decref(request);
}

TEST(wf_jsonrpc_is_request, invalid_request_without_method)
{
    json_t * request = json_object();
    json_object_set_new(request, "params", json_object());
    json_object_set_new(request, "id", json_integer(42));

    ASSERT_FALSE(wf_jsonrpc_is_request(request));

    json_decref(request);
}

TEST(wf_jsonrpc_is_request, invalid_request_due_to_invalid_method)
{
    json_t * request = json_object();
    json_object_set_new(request, "method", json_integer(42));
    json_object_set_new(request, "params", json_object());
    json_object_set_new(request, "id", json_integer(42));

    ASSERT_FALSE(wf_jsonrpc_is_request(request));

    json_decref(request);
}

TEST(wf_jsonrpc_is_request, invalid_request_without_params)
{
    json_t * request = json_object();
    json_object_set_new(request, "method", json_string("method"));
    json_object_set_new(request, "id", json_integer(42));

    ASSERT_FALSE(wf_jsonrpc_is_request(request));

    json_decref(request);
}

TEST(wf_jsonrpc_is_request, invalid_request_due_to_invalid_params)
{
    json_t * request = json_object();
    json_object_set_new(request, "methdo", json_string("method"));
    json_object_set_new(request, "params", json_string("params"));
    json_object_set_new(request, "id", json_integer(42));

    ASSERT_FALSE(wf_jsonrpc_is_request(request));

    json_decref(request);
}
