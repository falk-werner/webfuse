#include "webfuse/adapter/impl/operation/context.h"
#include "webfuse/adapter/impl/session.h"
#include <gtest/gtest.h>

TEST(wf_impl_operation_context, get_proxy)
{
    wf_jsonrpc_proxy * proxy = reinterpret_cast<wf_jsonrpc_proxy*>(42);
    wf_impl_operation_context context;
    context.proxy = proxy;

    ASSERT_EQ(proxy, wf_impl_operation_context_get_proxy(&context));
}

TEST(wf_impl_operation_context, get_proxy_fail_no_session)
{
    wf_impl_operation_context context;
    context.proxy = nullptr;

    ASSERT_EQ(nullptr, wf_impl_operation_context_get_proxy(&context));
    
}