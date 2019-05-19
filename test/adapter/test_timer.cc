#include <gtest/gtest.h>

#include <cstddef>

#include "msleep.hpp"
#include "webfuse/adapter/impl/time/timer.h"
#include "webfuse/adapter/impl/time/timeout_manager.h"

using std::size_t;
using webfuse_test::msleep;

namespace
{
    void on_timeout(struct wf_impl_timer * timer)
    {
        bool * triggered = reinterpret_cast<bool*>(timer->user_data);
        *triggered = true;
    }
}

TEST(timer, init)
{
    struct wf_impl_timeout_manager manager;
    struct wf_impl_timer timer;

    wf_impl_timeout_manager_init(&manager);
    wf_impl_timer_init(&timer, &manager);

    wf_impl_timer_cleanup(&timer);
    wf_impl_timeout_manager_cleanup(&manager);
}

TEST(timer, trigger)
{
    struct wf_impl_timeout_manager manager;
    struct wf_impl_timer timer;

    wf_impl_timeout_manager_init(&manager);
    wf_impl_timer_init(&timer, &manager);

    bool triggered = false;
    wf_impl_timer_start(&timer, wf_impl_timepoint_in_msec(250), &on_timeout, reinterpret_cast<void*>(&triggered));
    msleep(500);
    wf_impl_timeout_manager_check(&manager);

    ASSERT_TRUE(triggered);

    wf_impl_timer_cleanup(&timer);
    wf_impl_timeout_manager_cleanup(&manager);
}

TEST(timer, trigger_on_cleanup)
{
    struct wf_impl_timeout_manager manager;
    struct wf_impl_timer timer;

    wf_impl_timeout_manager_init(&manager);
    wf_impl_timer_init(&timer, &manager);

    bool triggered = false;
    wf_impl_timer_start(&timer, wf_impl_timepoint_in_msec(5 * 60 * 1000), &on_timeout, reinterpret_cast<void*>(&triggered));

    wf_impl_timeout_manager_cleanup(&manager);
    ASSERT_TRUE(triggered);

    wf_impl_timer_cleanup(&timer);
}

TEST(timer, cancel)
{
    struct wf_impl_timeout_manager manager;
    struct wf_impl_timer timer;

    wf_impl_timeout_manager_init(&manager);
    wf_impl_timer_init(&timer, &manager);

    bool triggered = false;
    wf_impl_timer_start(&timer, wf_impl_timepoint_in_msec(250), &on_timeout, &triggered);
    msleep(500);
    wf_impl_timer_cancel(&timer);
    wf_impl_timeout_manager_check(&manager);

    ASSERT_FALSE(triggered);
    
    wf_impl_timer_cleanup(&timer);
    wf_impl_timeout_manager_cleanup(&manager);
}

TEST(timer, cancel_multiple_timers)
{
    static size_t const count = 5;
    struct wf_impl_timeout_manager manager;
    struct wf_impl_timer timer[count];

    wf_impl_timeout_manager_init(&manager);

    bool triggered = false;
    for(size_t i = 0; i < count; i++)
    {
        wf_impl_timer_init(&timer[i], &manager);
        wf_impl_timer_start(&timer[i], wf_impl_timepoint_in_msec(0), &on_timeout, &triggered);
    }

    msleep(10);
    for(size_t i = 0; i < count; i++)
    {
        wf_impl_timer_cancel(&timer[i]);
    }

    wf_impl_timeout_manager_check(&manager);
    ASSERT_FALSE(triggered);
    
    for(size_t i = 0; i < count; i++)
    {
        wf_impl_timer_cleanup(&timer[0]);
    }
    wf_impl_timeout_manager_cleanup(&manager);
}

TEST(timer, multiple_timers)
{
    static size_t const count = 5;
    struct wf_impl_timeout_manager manager;
    struct wf_impl_timer timer[count];
    bool triggered[count];

    wf_impl_timeout_manager_init(&manager);

    for(size_t i = 0; i < count; i++)
    {
        wf_impl_timer_init(&timer[i], &manager);
        triggered[i] = false;
        wf_impl_timer_start(&timer[i], wf_impl_timepoint_in_msec(300 - (50 * i)), &on_timeout, &triggered[i]);
    }

    for(size_t i = 0; i < count; i++)
    {
        msleep(100);
        wf_impl_timeout_manager_check(&manager);
    }

    for(size_t i = 0; i < count; i++)
    {
        ASSERT_TRUE(triggered[i]);    
        wf_impl_timer_cleanup(&timer[i]);
    }

    wf_impl_timeout_manager_cleanup(&manager);
}
