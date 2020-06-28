#include <gtest/gtest.h>

#include "webfuse/impl/timer/timepoint.h"

#include <thread>
#include <chrono>

using namespace std::chrono_literals;

TEST(wf_timer_timepoint, now)
{
    wf_timer_timepoint start = wf_impl_timer_timepoint_now();
    std::this_thread::sleep_for(42ms);
    wf_timer_timepoint end = wf_impl_timer_timepoint_now();

    ASSERT_LT(start, end);
    ASSERT_LT(end, start + 500);
}

TEST(wf_timer_timepoint, in_msec)
{
    wf_timer_timepoint now = wf_impl_timer_timepoint_now();
    wf_timer_timepoint later = wf_impl_timer_timepoint_in_msec(42);

    ASSERT_LT(now, later);
    ASSERT_LT(later, now + 500);
}

TEST(wf_timer_timepoint, elapsed)
{
    wf_timer_timepoint now;
    
    now = wf_impl_timer_timepoint_now();
    ASSERT_TRUE(wf_impl_timer_timepoint_is_elapsed(now - 1));

    now = wf_impl_timer_timepoint_now();
    ASSERT_FALSE(wf_impl_timer_timepoint_is_elapsed(now + 500));
}
