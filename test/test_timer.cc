#include <gtest/gtest.h>

#include <cstddef>

#include "msleep.hpp"
#include "wsfs/adapter/impl/time/timer.h"
#include "wsfs/adapter/impl/time/timeout_manager.h"

using std::size_t;
using wsfs_test::msleep;

namespace
{
    void on_timeout(struct timer * timer)
    {
        bool * triggered = reinterpret_cast<bool*>(timer->user_data);
        *triggered = true;
    }
}

TEST(timer, init)
{
    struct timeout_manager manager;
    struct timer timer;

    timeout_manager_init(&manager);
    timer_init(&timer, &manager);

    timer_cleanup(&timer);
    timeout_manager_cleanup(&manager);
}

TEST(timer, trigger)
{
    struct timeout_manager manager;
    struct timer timer;

    timeout_manager_init(&manager);
    timer_init(&timer, &manager);

    bool triggered = false;
    timer_start(&timer, timepoint_in_msec(250), &on_timeout, reinterpret_cast<void*>(&triggered));
    msleep(500);
    timeout_manager_check(&manager);

    ASSERT_TRUE(triggered);

    timer_cleanup(&timer);
    timeout_manager_cleanup(&manager);
}

TEST(timer, cancel)
{
    struct timeout_manager manager;
    struct timer timer;

    timeout_manager_init(&manager);
    timer_init(&timer, &manager);

    bool triggered = false;
    timer_start(&timer, timepoint_in_msec(250), &on_timeout, &triggered);
    msleep(500);
    timer_cancel(&timer);
    timeout_manager_check(&manager);

    ASSERT_FALSE(triggered);
    
    timer_cleanup(&timer);
    timeout_manager_cleanup(&manager);
}

TEST(timer, multiple_timers)
{
    static size_t const count = 5;
    struct timeout_manager manager;
    struct timer timer[count];
    bool triggered[count];

    timeout_manager_init(&manager);

    for(size_t i = 0; i < count; i++)
    {
        timer_init(&timer[i], &manager);
        triggered[i] = false;
        timer_start(&timer[i], timepoint_in_msec(300 - (50 * i)), &on_timeout, &triggered[i]);
    }

    for(size_t i = 0; i < count; i++)
    {
        msleep(100);
        timeout_manager_check(&manager);
    }

    for(size_t i = 0; i < count; i++)
    {
        ASSERT_TRUE(triggered[i]);    
        timer_cleanup(&timer[i]);
    }

    timeout_manager_cleanup(&manager);
}
