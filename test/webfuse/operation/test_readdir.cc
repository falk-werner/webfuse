#include "webfuse/impl/operation/readdir.h"
#include "webfuse/impl/jsonrpc/error.h"

#include "webfuse/status.h"

#include "webfuse/test_util/json_doc.hpp"
#include "webfuse/mocks/mock_fuse.hpp"
#include "webfuse/mocks/mock_operation_context.hpp"
#include "webfuse/mocks/mock_jsonrpc_proxy.hpp"

#include <gtest/gtest.h>
#include <sstream>

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

TEST(wf_impl_operation_readdir, invoke_proxy)
{
    MockJsonRpcProxy proxy;
    EXPECT_CALL(proxy, wf_impl_jsonrpc_proxy_vinvoke(_,_,_,StrEq("readdir"),StrEq("si")))
        .Times(1).WillOnce(Invoke(free_context));

    MockOperationContext context;
    EXPECT_CALL(context, wf_impl_operation_context_get_proxy(_)).Times(1)
        .WillOnce(Return(reinterpret_cast<wf_jsonrpc_proxy*>(&proxy)));

    wf_impl_operation_context op_context;
    op_context.name = nullptr;
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_req_userdata(_)).Times(1).WillOnce(Return(&op_context));


    fuse_req_t request = nullptr;
    fuse_ino_t inode = 1;
    size_t size = 10;
    size_t offset = 0;
    fuse_file_info file_info;
    file_info.flags = 0;
    wf_impl_operation_readdir(request, inode, size, offset, &file_info);
}

TEST(wf_impl_operation_readdir, fail_rpc_null)
{
    MockOperationContext context;
    EXPECT_CALL(context, wf_impl_operation_context_get_proxy(_)).Times(1)
        .WillOnce(Return(nullptr));

    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_req_userdata(_)).Times(1).WillOnce(Return(nullptr));
    EXPECT_CALL(fuse, fuse_reply_err(_, ENOENT)).Times(1).WillOnce(Return(0));

    fuse_req_t request = nullptr;
    fuse_ino_t inode = 1;
    size_t size = 10;
    size_t offset = 0;
    fuse_file_info file_info;
    file_info.flags = 0;
    wf_impl_operation_readdir(request, inode, size, offset, &file_info);
}

TEST(wf_impl_operation_readdir, finished)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_buf(_,_,_)).Times(1).WillOnce(Return(0));

    JsonDoc result("[{\"name\": \"a.file\", \"inode\": 42}]");
    auto * context = reinterpret_cast<wf_impl_operation_readdir_context*>(malloc(sizeof(wf_impl_operation_readdir_context)));
    context->request = nullptr;
    context->size = 1;
    context->offset = 0;
    wf_impl_operation_readdir_finished(reinterpret_cast<void*>(context), result.root(), nullptr);
}

TEST(wf_impl_operation_readdir, finished_many_items)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_buf(_,_,_)).Times(1).WillOnce(Return(0));

    std::ostringstream stream;
    stream << "[" << "{\"name\": \"file_0.txt\", \"inode\": 1}";
    for(int i = 1; i < 100; i++)
    {
        stream << ",{\"name\": \"file_" << i << ".txt\", \"inode\": " << (i+1) << "}";
    }
    stream << "]";

    JsonDoc result(stream.str());
    auto * context = reinterpret_cast<wf_impl_operation_readdir_context*>(malloc(sizeof(wf_impl_operation_readdir_context)));
    context->request = nullptr;
    context->size = 100;
    context->offset = 0;
    wf_impl_operation_readdir_finished(reinterpret_cast<void*>(context), result.root(), nullptr);
}

TEST(wf_impl_operation_readdir, finished_read_after_end)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_buf(_,_,_)).Times(1).WillOnce(Return(0));

    JsonDoc result("[{\"name\": \"a.file\", \"inode\": 42}]");
    auto * context = reinterpret_cast<wf_impl_operation_readdir_context*>(malloc(sizeof(wf_impl_operation_readdir_context)));
    context->request = nullptr;
    context->size = 10;
    context->offset = 2;
    wf_impl_operation_readdir_finished(reinterpret_cast<void*>(context), result.root(), nullptr);
}

TEST(wf_impl_operation_readdir, finished_fail_error)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_buf(_,_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, ENOENT)).Times(1).WillOnce(Return(0));

    struct wf_jsonrpc_error * error = wf_impl_jsonrpc_error(WF_BAD, "");

    auto * context = reinterpret_cast<wf_impl_operation_readdir_context*>(malloc(sizeof(wf_impl_operation_readdir_context)));
    context->request = nullptr;
    context->size = 1;
    context->offset = 0;
    wf_impl_operation_readdir_finished(reinterpret_cast<void*>(context), nullptr, error);
    wf_impl_jsonrpc_error_dispose(error);
}

TEST(wf_impl_operation_readdir, finished_fail_invalid_result_type)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_buf(_,_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, ENOENT)).Times(1).WillOnce(Return(0));

    JsonDoc result("{}");
    auto * context = reinterpret_cast<wf_impl_operation_readdir_context*>(malloc(sizeof(wf_impl_operation_readdir_context)));
    context->request = nullptr;
    context->size = 1;
    context->offset = 0;
    wf_impl_operation_readdir_finished(reinterpret_cast<void*>(context), result.root(), nullptr);
}

TEST(wf_impl_operation_readdir, finished_fail_missing_name)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_buf(_,_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, ENOENT)).Times(1).WillOnce(Return(0));

    JsonDoc result("[{\"inode\": 42}]");
    auto * context = reinterpret_cast<wf_impl_operation_readdir_context*>(malloc(sizeof(wf_impl_operation_readdir_context)));
    context->request = nullptr;
    context->size = 1;
    context->offset = 0;
    wf_impl_operation_readdir_finished(reinterpret_cast<void*>(context), result.root(), nullptr);
}

TEST(wf_impl_operation_readdir, finished_fail_invalid_name_type)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_buf(_,_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, ENOENT)).Times(1).WillOnce(Return(0));

    JsonDoc result("[{\"name\": 42, \"inode\": 42}]");
    auto * context = reinterpret_cast<wf_impl_operation_readdir_context*>(malloc(sizeof(wf_impl_operation_readdir_context)));
    context->request = nullptr;
    context->size = 1;
    context->offset = 0;
    wf_impl_operation_readdir_finished(reinterpret_cast<void*>(context), result.root(), nullptr);
}

TEST(wf_impl_operation_readdir, finished_fail_missing_inode)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_buf(_,_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, ENOENT)).Times(1).WillOnce(Return(0));

    JsonDoc result("[{\"name\": \"a.file\"}]");
    auto * context = reinterpret_cast<wf_impl_operation_readdir_context*>(malloc(sizeof(wf_impl_operation_readdir_context)));
    context->request = nullptr;
    context->size = 1;
    context->offset = 0;
    wf_impl_operation_readdir_finished(reinterpret_cast<void*>(context), result.root(), nullptr);
}

TEST(wf_impl_operation_readdir, finished_fail_invalid_inode_type)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_buf(_,_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, ENOENT)).Times(1).WillOnce(Return(0));

    JsonDoc result("[{\"name\": \"a.file\", \"inode\": \"42\"}]");
    auto * context = reinterpret_cast<wf_impl_operation_readdir_context*>(malloc(sizeof(wf_impl_operation_readdir_context)));
    context->request = nullptr;
    context->size = 1;
    context->offset = 0;
    wf_impl_operation_readdir_finished(reinterpret_cast<void*>(context), result.root(), nullptr);
}

TEST(wf_impl_operation_readdir, finished_fail_invalid_item_type)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_buf(_,_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, ENOENT)).Times(1).WillOnce(Return(0));

    JsonDoc result("[\"item\"]");
    auto * context = reinterpret_cast<wf_impl_operation_readdir_context*>(malloc(sizeof(wf_impl_operation_readdir_context)));
    context->request = nullptr;
    context->size = 1;
    context->offset = 0;
    wf_impl_operation_readdir_finished(reinterpret_cast<void*>(context), result.root(), nullptr);
}
