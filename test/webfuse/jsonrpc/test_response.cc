#include "webfuse/impl/jsonrpc/response_intern.h"
#include "webfuse/impl/jsonrpc/error.h"
#include "webfuse/impl/json/node.h"
#include "webfuse/status.h"
#include "webfuse/test_util/json_doc.hpp"

#include <gtest/gtest.h>

using webfuse_test::JsonDoc;

TEST(wf_json_response, init_result)
{
    JsonDoc doc("{\"result\": 47, \"id\": 11}");

    struct wf_jsonrpc_response response;
    wf_impl_jsonrpc_response_init(&response, doc.root());

    ASSERT_EQ(nullptr, response.error);
    ASSERT_TRUE(wf_impl_json_is_int(response.result));
    ASSERT_EQ(47, wf_impl_json_int_get(response.result));
    ASSERT_EQ(11, response.id);

    wf_impl_jsonrpc_response_cleanup(&response);
}

TEST(wf_json_response, init_error)
{
    JsonDoc doc("{\"error\": {\"code\": 42, \"message\": \"Don't Panic!\"}, \"id\": 23}");

    struct wf_jsonrpc_response response;
    wf_impl_jsonrpc_response_init(&response, doc.root());

    ASSERT_EQ(42, wf_impl_jsonrpc_error_code(response.error));
    ASSERT_STREQ("Don't Panic!", wf_impl_jsonrpc_error_message(response.error));
    ASSERT_EQ(nullptr, response.result);
    ASSERT_EQ(23, response.id);

    wf_impl_jsonrpc_response_cleanup(&response);
}

TEST(wf_json_response, init_fail_missing_result_and_error)
{
    JsonDoc doc("{\"id\": 12}");

    struct wf_jsonrpc_response response;
    wf_impl_jsonrpc_response_init(&response, doc.root());

    ASSERT_EQ(WF_BAD_FORMAT, wf_impl_jsonrpc_error_code(response.error));
    ASSERT_EQ(nullptr, response.result);
    ASSERT_EQ(12, response.id);

    wf_impl_jsonrpc_response_cleanup(&response);
}

TEST(wf_json_response, init_fail_missing_id)
{
    JsonDoc doc("{\"result\": 47}");

    struct wf_jsonrpc_response response;
    wf_impl_jsonrpc_response_init(&response, doc.root());

    ASSERT_EQ(WF_BAD_FORMAT, wf_impl_jsonrpc_error_code(response.error));
    ASSERT_EQ(nullptr, response.result);
    ASSERT_EQ(-1, response.id);

    wf_impl_jsonrpc_response_cleanup(&response);
}

TEST(wf_json_response, init_fail_wrong_id_type)
{
    JsonDoc doc("{\"result\": 47, \"id\": \"42\"}");

    struct wf_jsonrpc_response response;
    wf_impl_jsonrpc_response_init(&response, doc.root());

    ASSERT_EQ(WF_BAD_FORMAT, wf_impl_jsonrpc_error_code(response.error));
    ASSERT_EQ(nullptr, response.result);
    ASSERT_EQ(-1, response.id);

    wf_impl_jsonrpc_response_cleanup(&response);
}

TEST(wf_json_response, init_fail_error_missing_code)
{
    JsonDoc doc("{\"error\": {\"message\": \"Don't Panic!\"}, \"id\": 23}");

    struct wf_jsonrpc_response response;
    wf_impl_jsonrpc_response_init(&response, doc.root());

    ASSERT_EQ(WF_BAD_FORMAT, wf_impl_jsonrpc_error_code(response.error));
    ASSERT_EQ(nullptr, response.result);
    ASSERT_EQ(23, response.id);

    wf_impl_jsonrpc_response_cleanup(&response);
}

TEST(wf_json_response, init_fail_error_wrong_code_type)
{
    JsonDoc doc("{\"error\": {\"code\": \"42\", \"message\": \"Don't Panic!\"}, \"id\": 23}");

    struct wf_jsonrpc_response response;
    wf_impl_jsonrpc_response_init(&response, doc.root());

    ASSERT_EQ(WF_BAD_FORMAT, wf_impl_jsonrpc_error_code(response.error));
    ASSERT_EQ(nullptr, response.result);
    ASSERT_EQ(23, response.id);

    wf_impl_jsonrpc_response_cleanup(&response);
}

TEST(wf_json_response, init_fail_error_wrong_type)
{
    JsonDoc doc("{\"error\": \"invalid error type\", \"id\": 23}");

    struct wf_jsonrpc_response response;
    wf_impl_jsonrpc_response_init(&response, doc.root());

    ASSERT_EQ(WF_BAD_FORMAT, wf_impl_jsonrpc_error_code(response.error));
    ASSERT_EQ(nullptr, response.result);
    ASSERT_EQ(23, response.id);

    wf_impl_jsonrpc_response_cleanup(&response);
}
