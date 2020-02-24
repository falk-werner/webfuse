#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "webfuse/adapter/mountpoint.h"

namespace
{
    class MockUserDataDisposer
    {
    public:
        MOCK_METHOD1(dispose, void(void * mountpoint));
    };

    MockUserDataDisposer * global_disposer = nullptr;

    void ondispose(void * user_data)
    {
        global_disposer->dispose(user_data);
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
    MockUserDataDisposer disposer;
    global_disposer = &disposer;

    wf_mountpoint * mountpoint = wf_mountpoint_create("/some/path");
    ASSERT_NE(nullptr, mountpoint);

    int value = 42;
    void * user_data = reinterpret_cast<void*>(&value);
    wf_mountpoint_set_userdata(mountpoint, user_data, ondispose);
    EXPECT_CALL(disposer, dispose(user_data)).Times(1);

    wf_mountpoint_dispose(mountpoint);
}