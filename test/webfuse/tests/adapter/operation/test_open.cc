#include "webfuse/adapter/impl/operation/open.h"

#include "webfuse/core/status.h"

#include "webfuse/mocks/mock_fuse.hpp"
#include "webfuse/mocks/mock_operation_context.hpp"
#include "webfuse/mocks/mock_jsonrpc_proxy.hpp"

#include <gtest/gtest.h>

using webfuse_test::MockJsonRpcProxy;
using webfuse_test::MockOperationContext;
using webfuse_test::FuseMock;
using testing::_;
using testing::Return;
using testing::StrEq;

TEST(wf_impl_operation_open, invoke_proxy)
{
    MockJsonRpcProxy proxy;
    EXPECT_CALL(proxy, wf_jsonrpc_proxy_vinvoke(_,_,_,StrEq("open"),StrEq("sii"))).Times(1);

    MockOperationContext context;
    EXPECT_CALL(context, wf_impl_operation_context_get_proxy(_)).Times(1)
        .WillOnce(Return(reinterpret_cast<wf_jsonrpc_proxy*>(&proxy)));

    wf_impl_operation_context op_context;
    op_context.name = nullptr;
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_req_userdata(_)).Times(1).WillOnce(Return(&op_context));


    fuse_req_t request = nullptr;
    fuse_ino_t inode = 1;
    fuse_file_info file_info;
    file_info.flags = 0;
    wf_impl_operation_open(request, inode, &file_info);
}

TEST(wf_impl_operation_open, fail_rpc_null)
{
    MockOperationContext context;
    EXPECT_CALL(context, wf_impl_operation_context_get_proxy(_)).Times(1)
        .WillOnce(Return(nullptr));

    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_req_userdata(_)).Times(1).WillOnce(Return(nullptr));
    EXPECT_CALL(fuse, fuse_reply_err(_, ENOENT)).Times(1).WillOnce(Return(0));

    fuse_req_t request = nullptr;
    fuse_ino_t inode = 1;
    fuse_file_info * file_info = nullptr;
    wf_impl_operation_open(request, inode, file_info);
}

TEST(wf_impl_operation_open, finished)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_open(_,_)).Times(1).WillOnce(Return(0));

    json_t * result = json_object();
    json_object_set_new(result, "handle", json_integer(42));
    wf_impl_operation_open_finished(nullptr, result, nullptr);
    json_decref(result);
}

TEST(wf_impl_operation_open, finished_fail_error)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_open(_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, ENOENT)).Times(1).WillOnce(Return(0));

    json_t * error = json_object();
    json_object_set_new(error, "code", json_integer(WF_BAD));
    wf_impl_operation_open_finished(nullptr, nullptr, error);
    json_decref(error);
}

TEST(wf_impl_operation_open, finished_fail_no_handle)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_open(_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, ENOENT)).Times(1).WillOnce(Return(0));

    json_t * result = json_object();
    wf_impl_operation_open_finished(nullptr, result, nullptr);
    json_decref(result);
}

TEST(wf_impl_operation_open, finished_fail_invalid_handle_type)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_open(_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, ENOENT)).Times(1).WillOnce(Return(0));

    json_t * result = json_object();
    json_object_set_new(result, "handle", json_string("42"));
    wf_impl_operation_open_finished(nullptr, result, nullptr);
    json_decref(result);
}
