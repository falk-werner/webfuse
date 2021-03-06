#include "webfuse/impl/operation/read.h"
#include "webfuse/impl/jsonrpc/error.h"

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
using testing::StrEq;

TEST(wf_impl_operation_read, invoke_proxy)
{
    MockJsonRpcProxy proxy;
    EXPECT_CALL(proxy, wf_impl_jsonrpc_proxy_vinvoke(_,_,_,StrEq("read"),StrEq("siiii"))).Times(1);

    MockOperationContext context;
    EXPECT_CALL(context, wf_impl_operation_context_get_proxy(_)).Times(1)
        .WillOnce(Return(reinterpret_cast<wf_jsonrpc_proxy*>(&proxy)));

    wf_impl_operation_context op_context;
    op_context.name = nullptr;
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_req_userdata(_)).Times(1).WillOnce(Return(&op_context));

    fuse_req_t request = nullptr;
    fuse_ino_t inode = 1;
    size_t size = 42;
    off_t offset = 0;
    fuse_file_info file_info;
    file_info.fh = 1;
    wf_impl_operation_read(request, inode, size, offset, &file_info);
}

TEST(wf_impl_operation_read, invoke_proxy_limit_size)
{
    MockJsonRpcProxy proxy;
    EXPECT_CALL(proxy, wf_impl_jsonrpc_proxy_vinvoke(_,_,_,StrEq("read"),StrEq("siiii"))).Times(0);

    MockOperationContext context;
    EXPECT_CALL(context, wf_impl_operation_context_get_proxy(_)).Times(1)
        .WillOnce(Return(reinterpret_cast<wf_jsonrpc_proxy*>(&proxy)));

    wf_impl_operation_context op_context;
    op_context.name = nullptr;
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_req_userdata(_)).Times(1).WillOnce(Return(&op_context));
    EXPECT_CALL(fuse, fuse_reply_err(_,_)).Times(1);

    fuse_req_t request = nullptr;
    fuse_ino_t inode = 1;
    size_t size = 100 * 1024 * 1024;
    off_t offset = 0;
    fuse_file_info file_info;
    file_info.fh = 1;
    wf_impl_operation_read(request, inode, size, offset, &file_info);
}

TEST(wf_impl_operation_read, fail_rpc_null)
{
    MockOperationContext context;
    EXPECT_CALL(context, wf_impl_operation_context_get_proxy(_)).Times(1)
        .WillOnce(Return(nullptr));

    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_req_userdata(_)).Times(1).WillOnce(Return(nullptr));
    EXPECT_CALL(fuse, fuse_reply_err(_, ENOENT)).Times(1).WillOnce(Return(0));

    fuse_req_t request = nullptr;
    fuse_ino_t inode = 1;
    size_t size = 42;
    off_t offset = 0;
    fuse_file_info * file_info = nullptr;
    wf_impl_operation_read(request, inode, size, offset, file_info);
}

TEST(wf_impl_operation_read, fill_buffer_identity)
{
    wf_status status;
    char text[] = "brummni";
    char * buffer = wf_impl_operation_read_transform(text, 8, "identity", 8, &status);
    ASSERT_EQ(WF_GOOD, status);
    ASSERT_STREQ("brummni", buffer);
}

TEST(wf_impl_operation_read, fill_buffer_identity_fail_inconsistent_size)
{
    wf_status status;
    char text[] = "brummni";
    wf_impl_operation_read_transform(text, 8, "identity", 7, &status);
    ASSERT_NE(WF_GOOD, status);
}

TEST(wf_impl_operation_read, fill_buffer_base64)
{
    wf_status status;
    char text[] = "YnJ1bW1uaQ==";
    char * buffer = wf_impl_operation_read_transform(text, 12, "base64", 7, &status);    

    ASSERT_EQ(WF_GOOD, status);
    ASSERT_EQ(0, strncmp("brummni", buffer, 7));
}

TEST(wf_impl_operation_read, fill_buffer_base64_fail_invalid_data)
{
    wf_status status;
    char text[] = "YnJ1bW1uaQ=A";
    wf_impl_operation_read_transform(text, 12, "base64", 8, &status);    
    ASSERT_NE(WF_GOOD, status);
}

TEST(wf_impl_operation_read, fill_buffer_empty)
{
    wf_status status;
    wf_impl_operation_read_transform(nullptr, 0, "identity", 0, &status);    

    ASSERT_EQ(WF_GOOD, status);
}

TEST(wf_impl_operation_read, fill_buffer_fail_invalid_format)
{
    wf_status status;
    char text[] = "some data";
    wf_impl_operation_read_transform(text, 9, "unknown", 9, &status);    
    ASSERT_NE(WF_GOOD, status);
}

TEST(wf_impl_operation_read, finished)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_buf(_,_,7)).Times(1).WillOnce(Return(0));

    JsonDoc result("{\"data\": \"brummni\", \"format\": \"identity\", \"count\": 7}");
    wf_impl_operation_read_finished(nullptr, result.root(), nullptr);
}

TEST(wf_impl_operation_read, finished_fail_no_data)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_buf(_,_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, _)).Times(1).WillOnce(Return(0));

    JsonDoc result("{\"format\": \"identity\", \"count\": 7}");
    wf_impl_operation_read_finished(nullptr, result.root(), nullptr);
}

TEST(wf_impl_operation_read, finished_fail_invalid_data_type)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_buf(_,_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, _)).Times(1).WillOnce(Return(0));

    JsonDoc result("{\"data\": 42, \"format\": \"identity\", \"count\": 7}");
    wf_impl_operation_read_finished(nullptr, result.root(), nullptr);
}

TEST(wf_impl_operation_read, finished_fail_no_format)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_buf(_,_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, _)).Times(1).WillOnce(Return(0));

    JsonDoc result("{\"data\": \"brummni\", \"count\": 7}");
    wf_impl_operation_read_finished(nullptr, result.root(), nullptr);
}

TEST(wf_impl_operation_read, finished_fail_invalid_format_type)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_buf(_,_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, _)).Times(1).WillOnce(Return(0));

    JsonDoc result("{\"data\": \"brummni\", \"format\": 42, \"count\": 7}");
    wf_impl_operation_read_finished(nullptr, result.root(), nullptr);
}

TEST(wf_impl_operation_read, finished_fail_no_count)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_buf(_,_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, _)).Times(1).WillOnce(Return(0));

    JsonDoc result("{\"data\": \"brummni\", \"format\": \"identity\"}");
    wf_impl_operation_read_finished(nullptr, result.root(), nullptr);
}

TEST(wf_impl_operation_read, finished_fail_invalid_count_type)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_buf(_,_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, _)).Times(1).WillOnce(Return(0));

    JsonDoc result("{\"data\": \"brummni\", \"format\": \"identity\", \"count\": \"7\"}");
    wf_impl_operation_read_finished(nullptr, result.root(), nullptr);
}

TEST(wf_impl_operation_read, finished_fail_error)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_buf(_,_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, _)).Times(1).WillOnce(Return(0));

    struct wf_jsonrpc_error * error = wf_impl_jsonrpc_error(WF_BAD, "");
    wf_impl_operation_read_finished(nullptr, nullptr, error);
    wf_impl_jsonrpc_error_dispose(error);
}
