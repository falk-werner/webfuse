#include <gtest/gtest.h>
#include "webfuse/core/jsonrpc/response.h"

TEST(wf_jsonrpc_is_response, valid_result)
{
    json_t * message = json_object();
    json_object_set_new(message, "result", json_object());
    json_object_set_new(message, "id", json_integer(42));

    ASSERT_TRUE(wf_jsonrpc_is_response(message));

    json_decref(message);
}

TEST(wf_jsonrpc_is_response, valid_result_string)
{
    json_t * message = json_object();
    json_object_set_new(message, "result", json_string("also valid"));
    json_object_set_new(message, "id", json_integer(42));

    ASSERT_TRUE(wf_jsonrpc_is_response(message));

    json_decref(message);
}

TEST(wf_jsonrpc_is_response, valid_error)
{
    json_t * message = json_object();
    json_object_set_new(message, "error", json_object());
    json_object_set_new(message, "id", json_integer(42));

    ASSERT_TRUE(wf_jsonrpc_is_response(message));

    json_decref(message);
}

TEST(wf_jsonrpc_is_response, invalid_null)
{
    ASSERT_FALSE(wf_jsonrpc_is_response(nullptr));
}

TEST(wf_jsonrpc_is_response, invalid_message)
{
    json_t * message = json_array();
    json_array_append_new(message, json_object());
    json_array_append_new(message, json_integer(42));

    ASSERT_FALSE(wf_jsonrpc_is_response(message));

    json_decref(message);
}

TEST(wf_jsonrpc_is_response, invalid_missing_id)
{
    json_t * message = json_object();
    json_object_set_new(message, "result", json_object());

    ASSERT_FALSE(wf_jsonrpc_is_response(message));

    json_decref(message);
}

TEST(wf_jsonrpc_is_response, invalid_id_wrong_type)
{
    json_t * message = json_object();
    json_object_set_new(message, "result", json_object());
    json_object_set_new(message, "id", json_string("42"));

    ASSERT_FALSE(wf_jsonrpc_is_response(message));

    json_decref(message);
}


TEST(wf_jsonrpc_is_response, invalid_missing_result_and_error)
{
    json_t * message = json_object();
    json_object_set_new(message, "id", json_integer(42));

    ASSERT_FALSE(wf_jsonrpc_is_response(message));

    json_decref(message);
}

TEST(wf_jsonrpc_is_response, invalid_error_wrong_type)
{
    json_t * message = json_object();
    json_object_set_new(message, "error", json_array());
    json_object_set_new(message, "id", json_integer(42));

    ASSERT_FALSE(wf_jsonrpc_is_response(message));

    json_decref(message);
}
