#include <gtest/gtest.h>

#include "msleep.hpp"
#include "wsfs/adapter/impl/time/timepoint.h"

using wsfs_test::msleep;

TEST(timepoint, now)
{
    wsfs_impl_timepoint start = wsfs_impl_timepoint_now();
    msleep(42);
    wsfs_impl_timepoint end = wsfs_impl_timepoint_now();

    ASSERT_LT(start, end);
    ASSERT_LT(end, start + 500);
}

TEST(timepoint, in_msec)
{
    wsfs_impl_timepoint now = wsfs_impl_timepoint_now();
    wsfs_impl_timepoint later = wsfs_impl_timepoint_in_msec(42);

    ASSERT_LT(now, later);
    ASSERT_LT(later, now + 500);
}

TEST(wsfs_impl_timepoint, elapsed)
{
    wsfs_impl_timepoint now;
    
    now = wsfs_impl_timepoint_now();
    ASSERT_TRUE(wsfs_impl_timepoint_is_elapsed(now - 1));

    now =wsfs_impl_timepoint_now();
    ASSERT_FALSE(wsfs_impl_timepoint_is_elapsed(now + 500));
}
