#include "webfuse/adapter/impl/operations.h"
#include "webfuse/mocks/mock_fuse.hpp"
#include "webfuse/adapter/impl/session.h"

#include <gtest/gtest.h>

using webfuse_test::FuseMock;
using testing::_;
using testing::Return;

TEST(wf_impl_operation_open, fail_rpc_null)
{
    wf_impl_session session;
    memset(&session, 0, sizeof(session));
    wf_impl_operations_context context =
        {&session, 1.0f, nullptr };

    FuseMock fuse;
    EXPECT_CALL(fuse, fuse_req_userdata(_)).Times(1)
        .WillOnce(Return(reinterpret_cast<void*>(&context)));
    EXPECT_CALL(fuse, fuse_reply_err(_, ENOENT)).Times(1)
        .WillOnce(Return(0));

    fuse_req_t request = nullptr;
    fuse_ino_t inode = 1;
    fuse_file_info * file_info = nullptr;
    wf_impl_operation_open(request, inode, file_info);
}