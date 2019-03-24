#include <gtest/gtest.h>

#include "msleep.hpp"
#include "wsfs/adapter/impl/time/timepoint.h"

using wsfs_test::msleep;

TEST(timepoint, now)
{
    timepoint start = timepoint_now();
    msleep(42);
    timepoint end = timepoint_now();

    ASSERT_LT(start, end);
    ASSERT_LT(end, start + 500);
}

TEST(timepoint, in_msec)
{
    timepoint now = timepoint_now();
    timepoint later = timepoint_in_msec(42);

    ASSERT_LT(now, later);
    ASSERT_LT(later, now + 500);
}

TEST(timepoint, elapsed)
{
    timepoint now;
    
    now = timepoint_now();
    ASSERT_TRUE(timepoint_is_elapsed(now - 1));

    now = timepoint_now();
    ASSERT_FALSE(timepoint_is_elapsed(now + 500));
}
