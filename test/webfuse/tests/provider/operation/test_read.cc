#include "webfuse/provider/impl/operation/read.h"
#include "webfuse/mocks/mock_request.hpp"
#include "webfuse/mocks/mock_provider.hpp"
#include "webfuse/mocks/fake_invokation_context.hpp"

#include <gtest/gtest.h>
#include <cstdlib>

using ::webfuse_test::MockProvider;
using ::webfuse_test::MockRequest;
using ::webfuse_test::ReadResultMatcher;
using ::webfuse_test::create_context;
using ::testing::_;
using ::testing::Invoke;
using ::testing::StrEq;

namespace
{

void free_request(wfp_request * request, ino_t, uint32_t, size_t ,size_t)
{
    free(request);
}

}

TEST(wfp_impl_read, invoke_provider)
{
    ino_t inode = 42;
    uint32_t handle = 5;
    size_t offset = 2;
    size_t length = 1;
    MockProvider provider;
    EXPECT_CALL(provider, read(_, inode, handle, offset, length)).Times(1)
        .WillOnce(Invoke(free_request));

    wfp_request request = {nullptr, nullptr, 0};
    wfp_impl_invokation_context context = create_context(provider, &request);

    json_t * params = json_array();
    json_array_append_new(params, json_string("test.filesystem"));
    json_array_append_new(params, json_integer(inode));
    json_array_append_new(params, json_integer(handle));
    json_array_append_new(params, json_integer(offset));
    json_array_append_new(params, json_integer(length));

    wfp_impl_read(&context, params, 42);
    json_decref(params);
}

TEST(wfp_impl_read, fail_invalid_param_count)
{
    MockProvider provider;
    EXPECT_CALL(provider, read(_, _, _, _, _)).Times(0);

    wfp_request request = {nullptr, nullptr, 0};
    wfp_impl_invokation_context context = create_context(provider, &request);

    json_t * params = json_array();
    json_array_append_new(params, json_string("test.filesystem"));
    json_array_append_new(params, json_integer(1));
    json_array_append_new(params, json_integer(2));
    json_array_append_new(params, json_integer(3));
    json_array_append_new(params, json_integer(4));
    json_array_append_new(params, json_integer(5));

    wfp_impl_read(&context, params, 42);
    json_decref(params);
}

TEST(wfp_impl_read, fail_invalid_inode_type)
{
    MockProvider provider;
    EXPECT_CALL(provider, read(_, _, _, _, _)).Times(0);

    wfp_request request = {nullptr, nullptr, 0};
    wfp_impl_invokation_context context = create_context(provider, &request);

    json_t * params = json_array();
    json_array_append_new(params, json_string("test.filesystem"));
    json_array_append_new(params, json_string("42"));
    json_array_append_new(params, json_integer(2));
    json_array_append_new(params, json_integer(3));
    json_array_append_new(params, json_integer(4));

    wfp_impl_read(&context, params, 42);
    json_decref(params);
}

TEST(wfp_impl_read, fail_invalid_handle_type)
{
    MockProvider provider;
    EXPECT_CALL(provider, read(_, _, _, _, _)).Times(0);

    wfp_request request = {nullptr, nullptr, 0};
    wfp_impl_invokation_context context = create_context(provider, &request);

    json_t * params = json_array();
    json_array_append_new(params, json_string("test.filesystem"));
    json_array_append_new(params, json_integer(1));
    json_array_append_new(params, json_string("42"));
    json_array_append_new(params, json_integer(3));
    json_array_append_new(params, json_integer(4));

    wfp_impl_read(&context, params, 42);
    json_decref(params);
}

TEST(wfp_impl_read, fail_invalid_offset_type)
{
    MockProvider provider;
    EXPECT_CALL(provider, read(_, _, _, _, _)).Times(0);

    wfp_request request = {nullptr, nullptr, 0};
    wfp_impl_invokation_context context = create_context(provider, &request);

    json_t * params = json_array();
    json_array_append_new(params, json_string("test.filesystem"));
    json_array_append_new(params, json_integer(1));
    json_array_append_new(params, json_integer(2));
    json_array_append_new(params, json_string("42"));
    json_array_append_new(params, json_integer(4));

    wfp_impl_read(&context, params, 42);
    json_decref(params);
}

TEST(wfp_impl_read, fail_invalid_length_type)
{
    MockProvider provider;
    EXPECT_CALL(provider, read(_, _, _, _, _)).Times(0);

    wfp_request request = {nullptr, nullptr, 0};
    wfp_impl_invokation_context context = create_context(provider, &request);

    json_t * params = json_array();
    json_array_append_new(params, json_string("test.filesystem"));
    json_array_append_new(params, json_integer(1));
    json_array_append_new(params, json_integer(2));
    json_array_append_new(params, json_integer(3));
    json_array_append_new(params, json_string("42"));

    wfp_impl_read(&context, params, 42);
    json_decref(params);
}

TEST(wfp_impl_read, default_responds_error)
{
    MockRequest request;
    auto * req = request.create_request(42);
    EXPECT_CALL(request, respond_error(_,42)).Times(1);

    wfp_impl_read_default(req, 0, 0, 1, 2, nullptr);
}

TEST(wfp_impl_read, respond)
{
    MockRequest request;
    auto * req = request.create_request(42);
    EXPECT_CALL(request, respond(ReadResultMatcher("d2Y=", "base64", 2), 42)).Times(1);

    char const data[] = "wf";
    wfp_impl_respond_read(req, data, 2);
}

TEST(wfp_impl_read, respond_empty)
{
    MockRequest request;
    auto * req = request.create_request(42);
    EXPECT_CALL(request, respond(ReadResultMatcher("", "identity", 0), 42)).Times(1);

    wfp_impl_respond_read(req, nullptr, 0);
}
