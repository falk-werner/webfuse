#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "webfuse/adapter/mountpoint.h"

namespace
{
    class MockMountpointDisposer
    {
    public:
        MOCK_METHOD1(ondispose, void(wf_mountpoint * mountpoint));
    };

    MockMountpointDisposer * global_disposer = nullptr;

    void ondispose(wf_mountpoint * mountpoint)
    {
        global_disposer->ondispose(mountpoint);
    }
}

TEST(mountpoint, get_path)
{
    wf_mountpoint * mountpoint = wf_mountpoint_create("/some/path");
    ASSERT_NE(nullptr, mountpoint);

    ASSERT_STREQ("/some/path", wf_mountpoint_get_path(mountpoint));

    wf_mountpoint_dispose(mountpoint);
}



TEST(mountpoint, ondispose)
{
    MockMountpointDisposer disposer;
    global_disposer = &disposer;

    wf_mountpoint * mountpoint = wf_mountpoint_create("/some/path");
    ASSERT_NE(nullptr, mountpoint);

    wf_mountpoint_set_ondispose(mountpoint, ondispose);
    EXPECT_CALL(disposer, ondispose(mountpoint)).Times(1);

    wf_mountpoint_dispose(mountpoint);
}