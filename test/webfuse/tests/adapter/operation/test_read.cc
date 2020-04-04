#include "webfuse/adapter/impl/operation/read.h"

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

TEST(wf_impl_operation_read, invoke_proxy)
{
    MockJsonRpcProxy proxy;
    EXPECT_CALL(proxy, wf_jsonrpc_proxy_vinvoke(_,_,_,StrEq("read"),StrEq("siiii"))).Times(1);

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
    EXPECT_CALL(proxy, wf_jsonrpc_proxy_vinvoke(_,_,_,StrEq("read"),StrEq("siiii"))).Times(1);

    MockOperationContext context;
    EXPECT_CALL(context, wf_impl_operation_context_get_proxy(_)).Times(1)
        .WillOnce(Return(reinterpret_cast<wf_jsonrpc_proxy*>(&proxy)));

    wf_impl_operation_context op_context;
    op_context.name = nullptr;
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_req_userdata(_)).Times(1).WillOnce(Return(&op_context));

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
    char * buffer = wf_impl_fill_buffer("brummni", 8, "identity", 8, &status);
    ASSERT_EQ(WF_GOOD, status);
    ASSERT_STREQ("brummni", buffer);
    free(buffer);
}

TEST(wf_impl_operation_read, fill_buffer_identity_fail_inconsistent_size)
{
    wf_status status;
    char * buffer = wf_impl_fill_buffer("brummni", 8, "identity", 7, &status);
    ASSERT_NE(WF_GOOD, status);
    ASSERT_EQ(nullptr, buffer);
}

TEST(wf_impl_operation_read, fill_buffer_base64)
{
    wf_status status;
    char * buffer = wf_impl_fill_buffer("YnJ1bW1uaQ==", 12, "base64", 7, &status);    

    ASSERT_EQ(WF_GOOD, status);
    ASSERT_EQ(0, strncmp("brummni", buffer, 7));
    free(buffer);
}

TEST(wf_impl_operation_read, fill_buffer_base64_fail_invalid_data)
{
    wf_status status;
    char * buffer = wf_impl_fill_buffer("YnJ1bW1uaQ=A", 12, "base64", 8, &status);    
    ASSERT_NE(WF_GOOD, status);
    ASSERT_EQ(nullptr, buffer);
}

TEST(wf_impl_operation_read, fill_buffer_empty)
{
    wf_status status;
    char * buffer = wf_impl_fill_buffer(nullptr, 0, "identity", 0, &status);    

    ASSERT_EQ(WF_GOOD, status);
    free(buffer);
}

TEST(wf_impl_operation_read, fill_buffer_fail_invalid_format)
{
    wf_status status;
    char * buffer = wf_impl_fill_buffer("some data", 9, "unknown", 9, &status);    
    ASSERT_NE(WF_GOOD, status);
    ASSERT_EQ(nullptr, buffer);
}

TEST(wf_impl_operation_read, finished)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_buf(_,_,7)).Times(1).WillOnce(Return(0));

    json_t * result = json_object();
    json_object_set_new(result, "data", json_string("brummni"));
    json_object_set_new(result, "format", json_string("identity"));
    json_object_set_new(result, "count", json_integer(7));
    wf_impl_operation_read_finished(nullptr, result, nullptr);
    json_decref(result);
}

TEST(wf_impl_operation_read, finished_fail_no_data)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_buf(_,_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, _)).Times(1).WillOnce(Return(0));

    json_t * result = json_object();
    json_object_set_new(result, "format", json_string("identity"));
    json_object_set_new(result, "count", json_integer(7));
    wf_impl_operation_read_finished(nullptr, result, nullptr);
    json_decref(result);
}

TEST(wf_impl_operation_read, finished_fail_invalid_data_type)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_buf(_,_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, _)).Times(1).WillOnce(Return(0));

    json_t * result = json_object();
    json_object_set_new(result, "data", json_integer(42));
    json_object_set_new(result, "format", json_string("identity"));
    json_object_set_new(result, "count", json_integer(7));
    wf_impl_operation_read_finished(nullptr, result, nullptr);
    json_decref(result);
}

TEST(wf_impl_operation_read, finished_fail_no_format)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_buf(_,_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, _)).Times(1).WillOnce(Return(0));

    json_t * result = json_object();
    json_object_set_new(result, "data", json_string("brummni"));
    json_object_set_new(result, "count", json_integer(7));
    wf_impl_operation_read_finished(nullptr, result, nullptr);
    json_decref(result);
}

TEST(wf_impl_operation_read, finished_fail_invalid_format_type)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_buf(_,_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, _)).Times(1).WillOnce(Return(0));

    json_t * result = json_object();
    json_object_set_new(result, "data", json_string("brummni"));
    json_object_set_new(result, "format", json_integer(42));
    json_object_set_new(result, "count", json_integer(7));
    wf_impl_operation_read_finished(nullptr, result, nullptr);
    json_decref(result);
}

TEST(wf_impl_operation_read, finished_fail_no_count)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_buf(_,_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, _)).Times(1).WillOnce(Return(0));

    json_t * result = json_object();
    json_object_set_new(result, "data", json_string("brummni"));
    json_object_set_new(result, "format", json_string("identity"));
    wf_impl_operation_read_finished(nullptr, result, nullptr);
    json_decref(result);
}

TEST(wf_impl_operation_read, finished_fail_invalid_count_type)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_buf(_,_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, _)).Times(1).WillOnce(Return(0));

    json_t * result = json_object();
    json_object_set_new(result, "data", json_string("brummni"));
    json_object_set_new(result, "format", json_string("identity"));
    json_object_set_new(result, "count", json_string("7"));
    wf_impl_operation_read_finished(nullptr, result, nullptr);
    json_decref(result);
}

TEST(wf_impl_operation_read, finished_fail_error)
{
    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_reply_buf(_,_,_)).Times(0);
    EXPECT_CALL(fuse, fuse_reply_err(_, _)).Times(1).WillOnce(Return(0));

    json_t * error = json_object();
    json_object_set_new(error, "code", json_integer(WF_BAD));
    wf_impl_operation_read_finished(nullptr, nullptr, error);
    json_decref(error);
}
