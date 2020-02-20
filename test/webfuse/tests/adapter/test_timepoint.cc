#include <gtest/gtest.h>

#include "webfuse/utils/msleep.hpp"
#include "webfuse/adapter/impl/time/timepoint.h"

using webfuse_test::msleep;

TEST(timepoint, now)
{
    wf_impl_timepoint start = wf_impl_timepoint_now();
    msleep(42);
    wf_impl_timepoint end = wf_impl_timepoint_now();

    ASSERT_LT(start, end);
    ASSERT_LT(end, start + 500);
}

TEST(timepoint, in_msec)
{
    wf_impl_timepoint now = wf_impl_timepoint_now();
    wf_impl_timepoint later = wf_impl_timepoint_in_msec(42);

    ASSERT_LT(now, later);
    ASSERT_LT(later, now + 500);
}

TEST(wf_impl_timepoint, elapsed)
{
    wf_impl_timepoint now;
    
    now = wf_impl_timepoint_now();
    ASSERT_TRUE(wf_impl_timepoint_is_elapsed(now - 1));

    now =wf_impl_timepoint_now();
    ASSERT_FALSE(wf_impl_timepoint_is_elapsed(now + 500));
}
