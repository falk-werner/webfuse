#include "webfuse/provider/impl/operation/open.h"
#include "webfuse/mocks/mock_request.hpp"
#include "webfuse/mocks/mock_provider.hpp"
#include "webfuse/mocks/fake_invokation_context.hpp"

#include <gtest/gtest.h>
#include <cstdlib>

using ::webfuse_test::MockProvider;
using ::webfuse_test::MockRequest;
using ::webfuse_test::OpenMatcher;
using ::webfuse_test::create_context;
using ::testing::_;
using ::testing::Invoke;
using ::testing::StrEq;

namespace
{

void free_request(wfp_request * request, ino_t, int)
{
    free(request);
}

}

TEST(wfp_impl_open, invoke_provider)
{
    ino_t inode = 42;
    int flags = 0;
    MockProvider provider;
    EXPECT_CALL(provider,open(_, inode, flags)).Times(1)
        .WillOnce(Invoke(free_request));

    wfp_request request = {nullptr, nullptr, 0};
    wfp_impl_invokation_context context = create_context(provider, &request);

    json_t * params = json_array();
    json_array_append_new(params, json_string("test.filesystem"));
    json_array_append_new(params, json_integer(inode));
    json_array_append_new(params, json_integer(flags));

    wfp_impl_open(&context, params, 42);
    json_decref(params);
}

TEST(wfp_impl_open, fail_invalid_param_count)
{
    MockProvider provider;
    EXPECT_CALL(provider,open(_, _, _)).Times(0);

    wfp_request request = {nullptr, nullptr, 0};
    wfp_impl_invokation_context context = create_context(provider, &request);

    json_t * params = json_array();
    json_array_append_new(params, json_string("test.filesystem"));
    json_array_append_new(params, json_integer(23));

    wfp_impl_open(&context, params, 42);
    json_decref(params);
}

TEST(wfp_impl_open, fail_invalid_inode_type)
{
    MockProvider provider;
    EXPECT_CALL(provider,open(_, _, _)).Times(0);

    wfp_request request = {nullptr, nullptr, 0};
    wfp_impl_invokation_context context = create_context(provider, &request);

    json_t * params = json_array();
    json_array_append_new(params, json_string("test.filesystem"));
    json_array_append_new(params, json_string(""));
    json_array_append_new(params, json_integer(0));

    wfp_impl_open(&context, params, 42);
    json_decref(params);
}

TEST(wfp_impl_open, fail_invalid_flags_type)
{
    MockProvider provider;
    EXPECT_CALL(provider,open(_, _, _)).Times(0);

    wfp_request request = {nullptr, nullptr, 0};
    wfp_impl_invokation_context context = create_context(provider, &request);

    json_t * params = json_array();
    json_array_append_new(params, json_string("test.filesystem"));
    json_array_append_new(params, json_integer(23));
    json_array_append_new(params, json_string(""));

    wfp_impl_open(&context, params, 42);
    json_decref(params);
}

TEST(wfp_impl_open, default_responds_error)
{
    MockRequest request;
    auto * req = request.create_request(42);
    EXPECT_CALL(request, respond_error(_,42)).Times(1);

    wfp_impl_open_default(req, 1, 0, nullptr);
}

TEST(wfp_impl_open, respond)
{
    MockRequest request;
    auto * req = request.create_request(42);
    EXPECT_CALL(request, respond(OpenMatcher(23), 42)).Times(1);

    wfp_impl_respond_open(req, 23);
}

