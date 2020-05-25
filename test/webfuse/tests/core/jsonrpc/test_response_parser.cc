#include <string>
#include <gtest/gtest.h>

#include "webfuse/core/jsonrpc/response_intern.h"


static void response_parse_str(
	std::string const & buffer,
	struct wf_jsonrpc_response * response)
{
	json_t * message = json_loadb(buffer.c_str(), buffer.size(), 0, nullptr);
	if (nullptr != message)
	{
		wf_jsonrpc_response_init(response, message);
		json_decref(message);
	}
}

TEST(response_parser, test)
{
	struct wf_jsonrpc_response response;

	// no object
	response_parse_str("[]", &response);
	ASSERT_NE(nullptr, response.error);
	ASSERT_EQ(-1, response.id);
	ASSERT_EQ(nullptr, response.result);
	wf_jsonrpc_response_cleanup(&response);

	// empty
	response_parse_str("{}", &response);
	ASSERT_NE(nullptr, response.error);
	ASSERT_EQ(-1, response.id);
	ASSERT_EQ(nullptr, response.result);
	wf_jsonrpc_response_cleanup(&response);

	// no data
	response_parse_str("{\"id\":42}", &response);
	ASSERT_NE(nullptr, response.error);
	ASSERT_EQ(42, response.id);
	ASSERT_EQ(nullptr, response.result);
	wf_jsonrpc_response_cleanup(&response);

	// custom error code
	response_parse_str("{\"error\":{\"code\": 42}, \"id\": 42}", &response);
	ASSERT_NE(nullptr, response.error);
	ASSERT_EQ(42, json_integer_value(json_object_get(response.error, "code")));
	ASSERT_EQ(42, response.id);
	ASSERT_EQ(nullptr, response.result);
	wf_jsonrpc_response_cleanup(&response);

	// valid response
	response_parse_str("{\"result\": true, \"id\": 42}", &response);
	ASSERT_EQ(nullptr, response.error);
	ASSERT_EQ(42, response.id);
	ASSERT_NE(nullptr, response.result);
	wf_jsonrpc_response_cleanup(&response);
}
