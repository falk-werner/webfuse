#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "webfuse/mountpoint.h"
#include "webfuse/impl/mountpoint.h"

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

TEST(mountpoint, default_options)
{
    wf_mountpoint * mountpoint = wf_mountpoint_create("/some/path");
    ASSERT_EQ(1, mountpoint->options.size);
    ASSERT_STREQ("", mountpoint->options.items[0]);
    ASSERT_EQ(nullptr, mountpoint->options.items[1]);

    wf_mountpoint_dispose(mountpoint);
}

TEST(mountpoint, add_mountoption)
{
    wf_mountpoint * mountpoint = wf_mountpoint_create("/some/path");
    wf_mountpoint_add_mountoption(mountpoint, "-o");
    wf_mountpoint_add_mountoption(mountpoint, "allow_other");

    ASSERT_EQ(3, mountpoint->options.size);
    ASSERT_STREQ("", mountpoint->options.items[0]);
    ASSERT_STREQ("-o", mountpoint->options.items[1]);
    ASSERT_STREQ("allow_other", mountpoint->options.items[2]);
    ASSERT_EQ(nullptr, mountpoint->options.items[3]);

    wf_mountpoint_dispose(mountpoint);
}

TEST(mountpoint, add_many_mountoption)
{
    wf_mountpoint * mountpoint = wf_mountpoint_create("/some/path");
    constexpr size_t count = 256;
    for (size_t i = 0; i < count; i++)
    {
        wf_mountpoint_add_mountoption(mountpoint, "any option");
    }

    ASSERT_EQ(count + 1, mountpoint->options.size);
    ASSERT_STREQ("", mountpoint->options.items[0]);
    for (size_t i = 0; i < count; i++)
    {
        ASSERT_STREQ("any option", mountpoint->options.items[i + 1]);
    }
    ASSERT_EQ(nullptr, mountpoint->options.items[count + 1]);

    wf_mountpoint_dispose(mountpoint);
}
