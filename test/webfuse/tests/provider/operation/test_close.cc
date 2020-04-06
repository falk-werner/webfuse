#include "webfuse/provider/impl/operation/close.h"
#include "webfuse/mocks/mock_provider.hpp"
#include "webfuse/mocks/fake_invokation_context.hpp"

#include <gtest/gtest.h>

using ::webfuse_test::MockProvider;
using ::webfuse_test::create_context;
using ::testing::_;

TEST(wfp_close, close)
{
    int inode = 42;
    int handle = 0xdeadbeef;
    int flags = 23;

    MockProvider provider;
    EXPECT_CALL(provider, close(inode, handle, flags)).Times(1);

    wfp_impl_invokation_context context = create_context(provider);

    json_t * params = json_array();
    json_array_append_new(params, json_string("test.filesystem"));
    json_array_append_new(params, json_integer(inode));
    json_array_append_new(params, json_integer(handle));
    json_array_append_new(params, json_integer(flags));

    wfp_impl_close(&context, params, 42);
    json_decref(params);
}

TEST(wfp_close, close_fail_invalid_param_count)
{
    MockProvider provider;
    EXPECT_CALL(provider, close(_,_,_)).Times(0);

    wfp_impl_invokation_context context = create_context(provider);

    json_t * params = json_array();
    wfp_impl_close(&context, params, 42);
    json_decref(params);
}

TEST(wfp_close, close_fail_inode_invalid_type)
{
    MockProvider provider;
    EXPECT_CALL(provider, close(_,_,_)).Times(0);

    wfp_impl_invokation_context context = create_context(provider);

    json_t * params = json_array();
    json_array_append_new(params, json_string("test.filesystem"));
    json_array_append_new(params, json_string("42"));
    json_array_append_new(params, json_integer(0));
    json_array_append_new(params, json_integer(0));

    wfp_impl_close(&context, params, 42);
    json_decref(params);
}

TEST(wfp_close, close_fail_handle_invalid_type)
{
    MockProvider provider;
    EXPECT_CALL(provider, close(_,_,_)).Times(0);

    wfp_impl_invokation_context context = create_context(provider);

    json_t * params = json_array();
    json_array_append_new(params, json_string("test.filesystem"));
    json_array_append_new(params, json_integer(0));
    json_array_append_new(params, json_string("42"));
    json_array_append_new(params, json_integer(0));

    wfp_impl_close(&context, params, 42);
    json_decref(params);
}

TEST(wfp_close, close_fail_flags_invalid_type)
{
    MockProvider provider;
    EXPECT_CALL(provider, close(_,_,_)).Times(0);

    wfp_impl_invokation_context context = create_context(provider);

    json_t * params = json_array();
    json_array_append_new(params, json_string("test.filesystem"));
    json_array_append_new(params, json_integer(0));
    json_array_append_new(params, json_integer(0));
    json_array_append_new(params, json_string("42"));

    wfp_impl_close(&context, params, 42);
    json_decref(params);
}


TEST(wfp_close, default_nop)
{
    wfp_impl_close_default(0, 0, 0, nullptr);
}