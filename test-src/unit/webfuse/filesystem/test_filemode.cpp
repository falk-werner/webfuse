#include "webfuse/filesystem/filemode.hpp"
#include <gtest/gtest.h>

using webfuse::filemode;

class filemode_test: public testing::TestWithParam<mode_t> { };

TEST_P(filemode_test, conversion)
{
    mode_t const expected = GetParam();
    auto value = filemode::from_mode(expected);
    ASSERT_EQ(expected, value.to_mode());
}

INSTANTIATE_TEST_CASE_P(filemode_value, filemode_test,
    testing::Values(
        S_IROTH, S_IWOTH, S_IXOTH,
        S_IRGRP, S_IWGRP, S_IXGRP,        
        S_IRUSR, S_IWUSR, S_IXUSR,
        S_ISUID, S_ISGID, S_ISVTX,     
        S_IFREG, S_IFCHR, S_IFBLK, S_IFDIR, S_IFIFO, S_IFLNK, S_IFSOCK,
        S_IFREG | 0644,
        S_IFDIR | 0755
    )
);