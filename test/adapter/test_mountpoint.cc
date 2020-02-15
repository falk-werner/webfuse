#include <gtest/gtest.h>
#include "webfuse/adapter/mountpoint.h"

TEST(mountpoint, get_path)
{
    wf_mountpoint * mountpoint = wf_mountpoint_create("/some/path");

    ASSERT_NE(nullptr, mountpoint);
    ASSERT_STREQ("/some/path", wf_mountpoint_get_path(mountpoint));

    wf_mountpoint_dispose(mountpoint);
}
