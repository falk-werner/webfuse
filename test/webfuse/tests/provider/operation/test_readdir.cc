#include "webfuse/provider/impl/operation/readdir.h"
#include "webfuse/mocks/mock_request.hpp"
#include "webfuse/mocks/mock_provider.hpp"
#include "webfuse/mocks/fake_invokation_context.hpp"
#include "webfuse/provider/dirbuffer.h"

#include <gtest/gtest.h>
#include <cstdlib>

using ::webfuse_test::MockProvider;
using ::webfuse_test::MockRequest;
using ::webfuse_test::ReaddirMatcher;
using ::webfuse_test::create_context;
using ::testing::_;
using ::testing::Invoke;

namespace
{

void free_request(wfp_request * request, ino_t)
{
    free(request);
}

}

TEST(wfp_impl_readdir, invoke_provider)
{
    ino_t inode = 23;
    MockProvider provider;
    EXPECT_CALL(provider,readdir(_, inode)).Times(1).WillOnce(Invoke(free_request));

    wfp_request request = {nullptr, nullptr, 0};
    wfp_impl_invokation_context context = create_context(provider, &request);

    json_t * params = json_array();
    json_array_append_new(params, json_string("test.filesystem"));
    json_array_append_new(params, json_integer(inode));

    wfp_impl_readdir(&context, params, 42);
    json_decref(params);
}

TEST(wfp_impl_readdir, fail_invalid_param_count)
{
    MockProvider provider;
    EXPECT_CALL(provider,readdir(_, _)).Times(0);

    wfp_request request = {nullptr, nullptr, 0};
    wfp_impl_invokation_context context = create_context(provider, &request);

    json_t * params = json_array();
    json_array_append_new(params, json_string("test.filesystem"));
    json_array_append_new(params, json_integer(1));
    json_array_append_new(params, json_integer(1));

    wfp_impl_readdir(&context, params, 42);
    json_decref(params);
}

TEST(wfp_impl_readdir, fail_invalid_inode_type)
{
    MockProvider provider;
    EXPECT_CALL(provider,readdir(_, _)).Times(0);

    wfp_request request = {nullptr, nullptr, 0};
    wfp_impl_invokation_context context = create_context(provider, &request);

    json_t * params = json_array();
    json_array_append_new(params, json_string("test.filesystem"));
    json_array_append_new(params, json_string("1"));

    wfp_impl_readdir(&context, params, 42);
    json_decref(params);
}

TEST(wfp_impl_readdir, default_responds_error)
{
    MockRequest request;
    auto * req = request.create_request(42);
    EXPECT_CALL(request, respond_error(_,42)).Times(1);

    wfp_impl_readdir_default(req, 0, nullptr);
}

TEST(wfp_impl_readdir, respond)
{
    char const item0[] = "some.file";
    char const * items[] = { item0, nullptr };

    MockRequest request;
    auto * req = request.create_request(42);
    EXPECT_CALL(request, respond(ReaddirMatcher(items), 42)).Times(1);

    wfp_dirbuffer * buffer = wfp_dirbuffer_create();
    wfp_dirbuffer_add(buffer, item0, 42);
    wfp_impl_respond_readdir(req, buffer);
    wfp_dirbuffer_dispose(buffer);

}
