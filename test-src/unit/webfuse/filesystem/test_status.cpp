#include "webfuse/filesystem/status.hpp"
#include <gtest/gtest.h>
#include <errno.h>

using webfuse::status;

class status_test: public testing::TestWithParam<int> { };

TEST_P(status_test, conversion)
{
    int const expected = GetParam();
    auto status = status::from_fusestatus(expected);
    ASSERT_EQ(expected, status.to_fusestatus());
}

INSTANTIATE_TEST_CASE_P(status_values, status_test,
    testing::Values(
        0, 1, 2, 3, 42,
        -E2BIG, -EACCES, -EBADF, -EBUSY, -EDESTADDRREQ,
        -EDQUOT, -EEXIST, -EFAULT, -EFBIG, -EINTR,
        -EINVAL, -EIO, -EISDIR, -ELOOP, -EMFILE,
        -ENAMETOOLONG, -ENFILE, -ENODATA, -ENODEV,
        -ENOENT, -ENOMEM, -ENOSPC, -ENOSYS, -ENOTDIR,
        -ENOTEMPTY, -ENOTSUP, -ENXIO, -EOVERFLOW, -EPERM,
        -EPIPE, -ERANGE, -EROFS, -EXDEV, -EWOULDBLOCK,
        -EAGAIN, -12345
    )
);
