#include <gtest/gtest.h>

#include <cstddef>

#include "msleep.hpp"
#include "wsfs/adapter/impl/time/timer.h"
#include "wsfs/adapter/impl/time/timeout_manager.h"

using std::size_t;
using wsfs_test::msleep;

namespace
{
    void on_timeout(struct wsfs_impl_timer * timer)
    {
        bool * triggered = reinterpret_cast<bool*>(timer->user_data);
        *triggered = true;
    }
}

TEST(timer, init)
{
    struct wsfs_impl_timeout_manager manager;
    struct wsfs_impl_timer timer;

    wsfs_impl_timeout_manager_init(&manager);
    wsfs_impl_timer_init(&timer, &manager);

    wsfs_impl_timer_cleanup(&timer);
    wsfs_impl_timeout_manager_cleanup(&manager);
}

TEST(timer, trigger)
{
    struct wsfs_impl_timeout_manager manager;
    struct wsfs_impl_timer timer;

    wsfs_impl_timeout_manager_init(&manager);
    wsfs_impl_timer_init(&timer, &manager);

    bool triggered = false;
    wsfs_impl_timer_start(&timer, wsfs_impl_timepoint_in_msec(250), &on_timeout, reinterpret_cast<void*>(&triggered));
    msleep(500);
    wsfs_impl_timeout_manager_check(&manager);

    ASSERT_TRUE(triggered);

    wsfs_impl_timer_cleanup(&timer);
    wsfs_impl_timeout_manager_cleanup(&manager);
}

TEST(timer, cancel)
{
    struct wsfs_impl_timeout_manager manager;
    struct wsfs_impl_timer timer;

    wsfs_impl_timeout_manager_init(&manager);
    wsfs_impl_timer_init(&timer, &manager);

    bool triggered = false;
    wsfs_impl_timer_start(&timer, wsfs_impl_timepoint_in_msec(250), &on_timeout, &triggered);
    msleep(500);
    wsfs_impl_timer_cancel(&timer);
    wsfs_impl_timeout_manager_check(&manager);

    ASSERT_FALSE(triggered);
    
    wsfs_impl_timer_cleanup(&timer);
    wsfs_impl_timeout_manager_cleanup(&manager);
}

TEST(timer, multiple_timers)
{
    static size_t const count = 5;
    struct wsfs_impl_timeout_manager manager;
    struct wsfs_impl_timer timer[count];
    bool triggered[count];

    wsfs_impl_timeout_manager_init(&manager);

    for(size_t i = 0; i < count; i++)
    {
        wsfs_impl_timer_init(&timer[i], &manager);
        triggered[i] = false;
        wsfs_impl_timer_start(&timer[i], wsfs_impl_timepoint_in_msec(300 - (50 * i)), &on_timeout, &triggered[i]);
    }

    for(size_t i = 0; i < count; i++)
    {
        msleep(100);
        wsfs_impl_timeout_manager_check(&manager);
    }

    for(size_t i = 0; i < count; i++)
    {
        ASSERT_TRUE(triggered[i]);    
        wsfs_impl_timer_cleanup(&timer[i]);
    }

    wsfs_impl_timeout_manager_cleanup(&manager);
}
