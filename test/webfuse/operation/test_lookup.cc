#include "webfuse/impl/operation/lookup.h"
#include "webfuse/impl/jsonrpc/error.h"

#include "webfuse/status.h"

#include "webfuse/test_util/json_doc.hpp"
#include "webfuse/mocks/mock_fuse.hpp"
#include "webfuse/mocks/mock_operation_context.hpp"
#include "webfuse/mocks/mock_jsonrpc_proxy.hpp"

#include <gtest/gtest.h>

using webfuse_test::JsonDoc;
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

TEST(wf_impl_operation_lookup, invoke_proxy)
{
    MockJsonRpcProxy proxy;
    EXPECT_CALL(proxy, wf_impl_jsonrpc_proxy_vinvoke(_,_,_,StrEq("lookup"),StrEq("sis"))).Times(1)
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
    wf_impl_operation_lookup(request, inode, "some.file");
}

TEST(wf_impl_operation_lookup, fail_rpc_null)
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
    wf_impl_operation_lookup(request, inode, "some.file");
}

TEST(wf_impl_operation_lookup, finished_file)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_entry(_,_)).Times(1).WillOnce(Return(0));

    JsonDoc result("{\"inode\": 42, \"mode\": 493, \"type\": \"file\"}");
    auto * context = reinterpret_cast<wf_impl_operation_lookup_context*>(malloc(sizeof(wf_impl_operation_lookup_context)));
    context->timeout = 1.0;
    context->gid = 0;
    context->uid = 0;
    wf_impl_operation_lookup_finished(context, result.root(), nullptr);
}

TEST(wf_impl_operation_lookup, finished_dir)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_entry(_,_)).Times(1).WillOnce(Return(0));

    JsonDoc result("{\"inode\": 42, \"mode\": 493, \"type\": \"dir\"}");
    auto * context = reinterpret_cast<wf_impl_operation_lookup_context*>(malloc(sizeof(wf_impl_operation_lookup_context)));
    context->timeout = 1.0;
    context->gid = 0;
    context->uid = 0;
    wf_impl_operation_lookup_finished(context, result.root(), nullptr);
}

TEST(wf_impl_operation_lookup, finished_unknown_type)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_entry(_,_)).Times(1).WillOnce(Return(0));

    JsonDoc result("{\"inode\": 42, \"mode\": 493, \"type\": \"unknown\"}");
    auto * context = reinterpret_cast<wf_impl_operation_lookup_context*>(malloc(sizeof(wf_impl_operation_lookup_context)));
    context->timeout = 1.0;
    context->gid = 0;
    context->uid = 0;
    wf_impl_operation_lookup_finished(context, result.root(), nullptr);
}

TEST(wf_impl_operation_lookup, finished_fail_missing_inode)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_open(_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, ENOENT)).Times(1).WillOnce(Return(0));

    JsonDoc result("{\"mode\": 493, \"type\": \"file\"}");
    auto * context = reinterpret_cast<wf_impl_operation_lookup_context*>(malloc(sizeof(wf_impl_operation_lookup_context)));
    context->timeout = 1.0;
    context->gid = 0;
    context->uid = 0;
    wf_impl_operation_lookup_finished(context, result.root(), nullptr);
}

TEST(wf_impl_operation_lookup, finished_fail_invalid_inode_type)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_open(_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, ENOENT)).Times(1).WillOnce(Return(0));

    JsonDoc result("{\"inode\": \"42\", \"mode\": 493, \"type\": \"file\"}");
    auto * context = reinterpret_cast<wf_impl_operation_lookup_context*>(malloc(sizeof(wf_impl_operation_lookup_context)));
    context->timeout = 1.0;
    context->gid = 0;
    context->uid = 0;
    wf_impl_operation_lookup_finished(context, result.root(), nullptr);
}

TEST(wf_impl_operation_lookup, finished_fail_missing_mode)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_open(_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, ENOENT)).Times(1).WillOnce(Return(0));

    JsonDoc result("{\"inode\": 42, \"type\": \"file\"}");
    auto * context = reinterpret_cast<wf_impl_operation_lookup_context*>(malloc(sizeof(wf_impl_operation_lookup_context)));
    context->timeout = 1.0;
    context->gid = 0;
    context->uid = 0;
    wf_impl_operation_lookup_finished(context, result.root(), nullptr);
}

TEST(wf_impl_operation_lookup, finished_fail_invalid_mode_type)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_open(_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, ENOENT)).Times(1).WillOnce(Return(0));

    JsonDoc result("{\"inode\": 42, \"mode\": \"0755\", \"type\": \"file\"}");
    auto * context = reinterpret_cast<wf_impl_operation_lookup_context*>(malloc(sizeof(wf_impl_operation_lookup_context)));
    context->timeout = 1.0;
    context->gid = 0;
    context->uid = 0;
    wf_impl_operation_lookup_finished(context, result.root(), nullptr);
}

TEST(wf_impl_operation_lookup, finished_fail_missing_type)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_open(_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, ENOENT)).Times(1).WillOnce(Return(0));

    JsonDoc result("{\"inode\": 42, \"mode\": 493}");
    auto * context = reinterpret_cast<wf_impl_operation_lookup_context*>(malloc(sizeof(wf_impl_operation_lookup_context)));
    context->timeout = 1.0;
    context->gid = 0;
    context->uid = 0;
    wf_impl_operation_lookup_finished(context, result.root(), nullptr);
}

TEST(wf_impl_operation_lookup, finished_fail_invalid_type_type)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_open(_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, ENOENT)).Times(1).WillOnce(Return(0));

    JsonDoc result("{\"inode\": 42, \"mode\": 493, \"type\": 42}");
    auto * context = reinterpret_cast<wf_impl_operation_lookup_context*>(malloc(sizeof(wf_impl_operation_lookup_context)));
    context->timeout = 1.0;
    context->gid = 0;
    context->uid = 0;
    wf_impl_operation_lookup_finished(context, result.root(), nullptr);
}

TEST(wf_impl_operation_lookup, finished_error)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_open(_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, ENOENT)).Times(1).WillOnce(Return(0));

    struct wf_jsonrpc_error * error = wf_impl_jsonrpc_error(WF_BAD, "");

    auto * context = reinterpret_cast<wf_impl_operation_lookup_context*>(malloc(sizeof(wf_impl_operation_lookup_context)));
    context->timeout = 1.0;
    context->gid = 0;
    context->uid = 0;
    wf_impl_operation_lookup_finished(context, nullptr, error);
    wf_impl_jsonrpc_error_dispose(error);
}
