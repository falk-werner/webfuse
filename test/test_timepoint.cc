#include <gtest/gtest.h>
#include <unistd.h>
#include "wsfs/time/timepoint.h"


TEST(timepoint, now)
{
    wsfs_timepoint start = wsfs_timepoint_now();
    usleep(42 * 1000);
    wsfs_timepoint end = wsfs_timepoint_now();

    ASSERT_LT(start, end);
    ASSERT_LT(end, start + 500);
}

TEST(timepoint, in_msec)
{
    wsfs_timepoint now = wsfs_timepoint_now();
    wsfs_timepoint later = wsfs_timepoint_in_msec(42);

    ASSERT_LT(now, later);
    ASSERT_LT(later, now + 500);
}

TEST(timepoint, elapsed)
{
    wsfs_timepoint now;
    
    now = wsfs_timepoint_now();
    ASSERT_TRUE(wsfs_timepoint_is_elapsed(now - 1));

    now = wsfs_timepoint_now();
    ASSERT_FALSE(wsfs_timepoint_is_elapsed(now + 500));
}
