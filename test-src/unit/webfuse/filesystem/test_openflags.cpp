#include "webfuse/filesystem/openflags.hpp"
#include <gtest/gtest.h>
#include <fcntl.h>

using webfuse::openflags;

class openflags_test: public testing::TestWithParam<int> { };

TEST_P(openflags_test, conversion)
{
    int const expected = GetParam();
    auto flags = openflags::from_int(expected);
    ASSERT_EQ(expected, flags.to_int());
}

INSTANTIATE_TEST_CASE_P(openflags_values, openflags_test,
    testing::Values<>(
        O_RDONLY, O_WRONLY, O_RDWR, O_CLOEXEC, O_CREAT,
        O_DIRECT, O_DIRECTORY, O_EXCL, O_NOCTTY, O_NOFOLLOW,
        O_TRUNC, O_ASYNC, O_LARGEFILE, O_NOATIME, O_NONBLOCK,
        O_NDELAY, O_SYNC,
        O_WRONLY | O_CREAT | O_TRUNC
    )
);
