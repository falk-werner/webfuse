#include "webfuse/adapter/impl/operation/getattr.h"

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
using testing::Invoke;
using testing::StrEq;

namespace
{

void free_context(
    struct wf_jsonrpc_proxy * ,
    wf_jsonrpc_proxy_finished_fn * ,
    void * user_data,
    char const * ,
    char const *)
{
    free(user_data);    
}

}

TEST(wf_impl_operation_getattr, invoke_proxy)
{
    MockJsonRpcProxy proxy;
    EXPECT_CALL(proxy, wf_jsonrpc_proxy_vinvoke(_,_,_,StrEq("getattr"),StrEq("si"))).Times(1)
        .WillOnce(Invoke(free_context));

    MockOperationContext context;
    EXPECT_CALL(context, wf_impl_operation_context_get_proxy(_)).Times(1)
        .WillOnce(Return(reinterpret_cast<wf_jsonrpc_proxy*>(&proxy)));

    wf_impl_operation_context op_context;
    op_context.name = nullptr;
    fuse_ctx fuse_context;
    fuse_context.gid = 0;
    fuse_context.uid = 0;
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_req_ctx(_)).Times(1).WillOnce(Return(&fuse_context));
    EXPECT_CALL(fuse, fuse_req_userdata(_)).Times(1).WillOnce(Return(&op_context));


    fuse_req_t request = nullptr;
    fuse_ino_t inode = 1;
    fuse_file_info * file_info = nullptr;
    wf_impl_operation_getattr(request, inode, file_info);
}

TEST(wf_impl_operation_getattr, fail_rpc_null)
{
    MockOperationContext context;
    EXPECT_CALL(context, wf_impl_operation_context_get_proxy(_)).Times(1)
        .WillOnce(Return(nullptr));

    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_req_ctx(_)).Times(1).WillOnce(Return(nullptr));
    EXPECT_CALL(fuse, fuse_req_userdata(_)).Times(1).WillOnce(Return(nullptr));
    EXPECT_CALL(fuse, fuse_reply_err(_, ENOENT)).Times(1).WillOnce(Return(0));

    fuse_req_t request = nullptr;
    fuse_ino_t inode = 1;
    fuse_file_info * file_info = nullptr;
    wf_impl_operation_getattr(request, inode, file_info);
}

TEST(wf_impl_operation_getattr, finished_file)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_attr(_,_,_)).Times(1).WillOnce(Return(0));

    json_t * result = json_object();
    json_object_set_new(result, "mode", json_integer(0755));
    json_object_set_new(result, "type", json_string("file"));

    auto * context = reinterpret_cast<wf_impl_operation_getattr_context*>(malloc(sizeof(wf_impl_operation_getattr_context)));
    context->inode = 1;
    context->gid = 0;
    context->uid = 0;
    wf_impl_operation_getattr_finished(context, result, nullptr);
    json_decref(result);
}

TEST(wf_impl_operation_getattr, finished_dir)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_attr(_,_,_)).Times(1).WillOnce(Return(0));

    json_t * result = json_object();
    json_object_set_new(result, "mode", json_integer(0755));
    json_object_set_new(result, "type", json_string("dir"));

    auto * context = reinterpret_cast<wf_impl_operation_getattr_context*>(malloc(sizeof(wf_impl_operation_getattr_context)));
    context->inode = 1;
    context->gid = 0;
    context->uid = 0;
    wf_impl_operation_getattr_finished(context, result, nullptr);
    json_decref(result);
}

TEST(wf_impl_operation_getattr, finished_unknown_type)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_attr(_,_,_)).Times(1).WillOnce(Return(0));

    json_t * result = json_object();
    json_object_set_new(result, "mode", json_integer(0755));
    json_object_set_new(result, "type", json_string("unknown"));

    auto * context = reinterpret_cast<wf_impl_operation_getattr_context*>(malloc(sizeof(wf_impl_operation_getattr_context)));
    context->inode = 1;
    context->gid = 0;
    context->uid = 0;
    wf_impl_operation_getattr_finished(context, result, nullptr);
    json_decref(result);
}

TEST(wf_impl_operation_getattr, finished_fail_missing_mode)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_open(_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, ENOENT)).Times(1).WillOnce(Return(0));

    json_t * result = json_object();
    json_object_set_new(result, "type", json_string("file"));

    auto * context = reinterpret_cast<wf_impl_operation_getattr_context*>(malloc(sizeof(wf_impl_operation_getattr_context)));
    context->inode = 1;
    context->gid = 0;
    context->uid = 0;
    wf_impl_operation_getattr_finished(context, result, nullptr);
    json_decref(result);
}

TEST(wf_impl_operation_getattr, finished_fail_invalid_mode_type)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_open(_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, ENOENT)).Times(1).WillOnce(Return(0));

    json_t * result = json_object();
    json_object_set_new(result, "mode", json_string("0755"));
    json_object_set_new(result, "type", json_string("file"));

    auto * context = reinterpret_cast<wf_impl_operation_getattr_context*>(malloc(sizeof(wf_impl_operation_getattr_context)));
    context->inode = 1;
    context->gid = 0;
    context->uid = 0;
    wf_impl_operation_getattr_finished(context, result, nullptr);
    json_decref(result);
}

TEST(wf_impl_operation_getattr, finished_fail_type_mode)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_open(_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, ENOENT)).Times(1).WillOnce(Return(0));

    json_t * result = json_object();
    json_object_set_new(result, "mode", json_integer(0755));

    auto * context = reinterpret_cast<wf_impl_operation_getattr_context*>(malloc(sizeof(wf_impl_operation_getattr_context)));
    context->inode = 1;
    context->gid = 0;
    context->uid = 0;
    wf_impl_operation_getattr_finished(context, result, nullptr);
    json_decref(result);
}

TEST(wf_impl_operation_getattr, finished_fail_invalid_type_type)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_open(_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, ENOENT)).Times(1).WillOnce(Return(0));

    json_t * result = json_object();
    json_object_set_new(result, "mode", json_integer(0755));
    json_object_set_new(result, "type", json_integer(42));

    auto * context = reinterpret_cast<wf_impl_operation_getattr_context*>(malloc(sizeof(wf_impl_operation_getattr_context)));
    context->inode = 1;
    context->gid = 0;
    context->uid = 0;
    wf_impl_operation_getattr_finished(context, result, nullptr);
    json_decref(result);
}

TEST(wf_impl_operation_getattr, finished_error)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_open(_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, ENOENT)).Times(1).WillOnce(Return(0));

    json_t * error = json_object();
    json_object_set_new(error, "code", json_integer(WF_BAD));

    auto * context = reinterpret_cast<wf_impl_operation_getattr_context*>(malloc(sizeof(wf_impl_operation_getattr_context)));
    context->inode = 1;
    context->gid = 0;
    context->uid = 0;
    wf_impl_operation_getattr_finished(context, nullptr, error);
    json_decref(error);
}
