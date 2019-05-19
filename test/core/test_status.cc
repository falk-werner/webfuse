#include <gtest/gtest.h>
#include "webfuse/core/status_intern.h"

TEST(wf_status, tostring)
{
    ASSERT_STREQ("Good", wf_status_tostring(WF_GOOD));
    ASSERT_STREQ("Bad", wf_status_tostring(WF_BAD));
    ASSERT_STREQ("Bad (not implemented)", wf_status_tostring(WF_BAD_NOTIMPLEMENTED));
    ASSERT_STREQ("Bad (busy)", wf_status_tostring(WF_BAD_BUSY));
    ASSERT_STREQ("Bad (timeout)", wf_status_tostring(WF_BAD_TIMEOUT));
    ASSERT_STREQ("Bad (format)", wf_status_tostring(WF_BAD_FORMAT));
    ASSERT_STREQ("Bad (no entry)", wf_status_tostring(WF_BAD_NOENTRY));
    ASSERT_STREQ("Bad (access denied)", wf_status_tostring(WF_BAD_ACCESS_DENIED));

    ASSERT_STREQ("Bad (unknown)", wf_status_tostring(-1));
}

TEST(wf_status, to_rc)
{
    ASSERT_EQ(0, wf_status_to_rc(WF_GOOD));
    ASSERT_EQ(-ENOENT, wf_status_to_rc(WF_BAD));
    ASSERT_EQ(-ENOSYS, wf_status_to_rc(WF_BAD_NOTIMPLEMENTED));
    ASSERT_EQ(-ENOENT, wf_status_to_rc(WF_BAD_BUSY));
    ASSERT_EQ(-ETIMEDOUT, wf_status_to_rc(WF_BAD_TIMEOUT));
    ASSERT_EQ(-ENOENT, wf_status_to_rc(WF_BAD_FORMAT));
    ASSERT_EQ(-ENOENT, wf_status_to_rc(WF_BAD_NOENTRY));
    ASSERT_EQ(-EACCES, wf_status_to_rc(WF_BAD_ACCESS_DENIED));

    ASSERT_EQ(-ENOENT, wf_status_to_rc(-1));
}