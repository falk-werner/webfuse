#include <gtest/gtest.h>

#include "webfuse/utils/msleep.hpp"
#include "wf/timer/impl/timepoint.h"

using webfuse_test::msleep;

TEST(wf_timer_timepoint, now)
{
    wf_timer_timepoint start = wf_timer_impl_timepoint_now();
    msleep(42);
    wf_timer_timepoint end = wf_timer_impl_timepoint_now();

    ASSERT_LT(start, end);
    ASSERT_LT(end, start + 500);
}

TEST(wf_timer_timepoint, in_msec)
{
    wf_timer_timepoint now = wf_timer_impl_timepoint_now();
    wf_timer_timepoint later = wf_timer_impl_timepoint_in_msec(42);

    ASSERT_LT(now, later);
    ASSERT_LT(later, now + 500);
}

TEST(wf_timer_timepoint, elapsed)
{
    wf_timer_timepoint now;
    
    now = wf_timer_impl_timepoint_now();
    ASSERT_TRUE(wf_timer_impl_timepoint_is_elapsed(now - 1));

    now = wf_timer_impl_timepoint_now();
    ASSERT_FALSE(wf_timer_impl_timepoint_is_elapsed(now + 500));
}
