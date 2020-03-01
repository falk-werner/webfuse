#include <gtest/gtest.h>
#include "wf/jsonrpc/impl/response.h"
#include "webfuse/core/status.h"
#include "webfuse/core/json_util.h"

TEST(wf_json_response, init_result)
{
    json_t * message = json_object();
    json_object_set_new(message, "result", json_integer(47));
    json_object_set_new(message, "id", json_integer(11));

    struct wf_jsonrpc_response response;
    wf_jsonrpc_impl_response_init(&response, message);

    ASSERT_EQ(nullptr, response.error);
    ASSERT_TRUE(json_is_integer(response.result));
    ASSERT_EQ(47, json_integer_value(response.result));
    ASSERT_EQ(11, response.id);

    wf_jsonrpc_impl_response_cleanup(&response);
    json_decref(message);
}

TEST(wf_json_response, init_error)
{
    json_t * message = json_object();
    json_t * err = json_object();
    json_object_set_new(err, "code", json_integer(WF_BAD_ACCESS_DENIED));
    json_object_set_new(err, "message", json_string("access denied"));
    json_object_set_new(message, "error", err);
    json_object_set_new(message, "id", json_integer(23));

    struct wf_jsonrpc_response response;
    wf_jsonrpc_impl_response_init(&response, message);

    ASSERT_EQ(WF_BAD_ACCESS_DENIED, wf_impl_jsonrpc_get_status(response.error)) << json_string_value(json_object_get(response.error, "message"));
    ASSERT_EQ(nullptr, response.result);
    ASSERT_EQ(23, response.id);

    wf_jsonrpc_impl_response_cleanup(&response);
    json_decref(message);
}

TEST(wf_json_response, init_format_error)
{
    json_t * message = json_object();
    json_object_set_new(message, "id", json_integer(12));

    struct wf_jsonrpc_response response;
    wf_jsonrpc_impl_response_init(&response, message);

    ASSERT_EQ(WF_BAD_FORMAT, wf_impl_jsonrpc_get_status(response.error));
    ASSERT_EQ(nullptr, response.result);
    ASSERT_EQ(12, response.id);

    wf_jsonrpc_impl_response_cleanup(&response);
    json_decref(message);
}
