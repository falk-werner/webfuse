
#include "webfuse/impl/jsonrpc/response_intern.h"
#include "webfuse/impl/jsonrpc/error.h"
#include "webfuse/test_util/json_doc.hpp"

#include <gtest/gtest.h>
#include <string>

using webfuse_test::JsonDoc;

TEST(response_parser, invalid_no_object)
{
	JsonDoc doc("[]");
	struct wf_jsonrpc_response response;
	wf_impl_jsonrpc_response_init(&response, doc.root());

	ASSERT_NE(nullptr, response.error);
	ASSERT_EQ(-1, response.id);
	ASSERT_EQ(nullptr, response.result);
	wf_impl_jsonrpc_response_cleanup(&response);
}

TEST(response_parser, invalid_empty_object)
{
	JsonDoc doc("{}");
	struct wf_jsonrpc_response response;
	wf_impl_jsonrpc_response_init(&response, doc.root());

	ASSERT_NE(nullptr, response.error);
	ASSERT_EQ(-1, response.id);
	ASSERT_EQ(nullptr, response.result);
	wf_impl_jsonrpc_response_cleanup(&response);
}

TEST(response_parser, invalid_missin_result_and_error)
{
	JsonDoc doc("{\"id\":42}");
	struct wf_jsonrpc_response response;
	wf_impl_jsonrpc_response_init(&response, doc.root());

	ASSERT_NE(nullptr, response.error);
	ASSERT_EQ(42, response.id);
	ASSERT_EQ(nullptr, response.result);
	wf_impl_jsonrpc_response_cleanup(&response);
}

TEST(response_parser, custom_error_code)
{
	JsonDoc doc("{\"error\":{\"code\": 42}, \"id\": 42}");
	struct wf_jsonrpc_response response;
	wf_impl_jsonrpc_response_init(&response, doc.root());

	ASSERT_NE(nullptr, response.error);
	ASSERT_EQ(42, wf_impl_jsonrpc_error_code(response.error));
	ASSERT_EQ(42, response.id);
	ASSERT_EQ(nullptr, response.result);
	wf_impl_jsonrpc_response_cleanup(&response);
}

TEST(response_parser, valid_response)
{
	JsonDoc doc("{\"result\": true, \"id\": 42}");
	struct wf_jsonrpc_response response;
	wf_impl_jsonrpc_response_init(&response, doc.root());

	ASSERT_EQ(nullptr, response.error);
	ASSERT_EQ(42, response.id);
	ASSERT_NE(nullptr, response.result);
	wf_impl_jsonrpc_response_cleanup(&response);
}
