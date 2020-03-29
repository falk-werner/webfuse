#include "webfuse/provider/impl/operation/getattr.h"
#include "webfuse/mocks/mock_request.hpp"
#include <gtest/gtest.h>

using ::webfuse_test::MockRequest;
using ::webfuse_test::GetAttrMatcher;
using ::testing::_;

TEST(wfp_impl_getattr, default_responds_error)
{
    MockRequest request;
    auto * req = request.create_request(42);
    EXPECT_CALL(request, respond_error(_,42)).Times(1);

    wfp_impl_getattr_default(req, 0, nullptr);
}

TEST(wfp_impl_getattr, respond_file)
{
    MockRequest request;
    auto * req = request.create_request(42);
    EXPECT_CALL(request, respond(GetAttrMatcher(23, 0754, "file"), 42)).Times(1);

    struct stat buffer;
    memset(&buffer, 0, sizeof(buffer));
    buffer.st_ino = 23;
    buffer.st_mode = S_IFREG | 0754;
    wfp_impl_respond_getattr(req, &buffer);
}

TEST(wfp_impl_getattr, respond_dir)
{
    MockRequest request;
    auto * req = request.create_request(42);
    EXPECT_CALL(request, respond(GetAttrMatcher(23, 0754, "dir"), 42)).Times(1);

    struct stat buffer;
    memset(&buffer, 0, sizeof(buffer));
    buffer.st_ino = 23;
    buffer.st_mode = S_IFDIR | 0754;
    wfp_impl_respond_getattr(req, &buffer);
}