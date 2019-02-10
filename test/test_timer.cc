#include <gtest/gtest.h>

#include <cstddef>
#include <unistd.h>

#include "wsfs/time/timer.h"
#include "wsfs/time/timeout_manager.h"

using std::size_t;

namespace
{
    void on_timeout(struct wsfs_timer * timer)
    {
        bool * triggered = reinterpret_cast<bool*>(timer->user_data);
        *triggered = true;
    }
}

TEST(timer, init)
{
    struct wsfs_timeout_manager manager;
    struct wsfs_timer timer;

    wsfs_timeout_manager_init(&manager);
    wsfs_timer_init(&timer, &manager);

    wsfs_timer_cleanup(&timer);
    wsfs_timeout_manager_cleanup(&manager);
}

TEST(timer, trigger)
{
    struct wsfs_timeout_manager manager;
    struct wsfs_timer timer;

    wsfs_timeout_manager_init(&manager);
    wsfs_timer_init(&timer, &manager);

    bool triggered = false;
    wsfs_timer_start(&timer, wsfs_timepoint_in_msec(250), &on_timeout, reinterpret_cast<void*>(&triggered));
    usleep(500 * 1000);
    wsfs_timeout_manager_check(&manager);

    ASSERT_TRUE(triggered);

    wsfs_timer_cleanup(&timer);
    wsfs_timeout_manager_cleanup(&manager);
}

TEST(timer, cancel)
{
    struct wsfs_timeout_manager manager;
    struct wsfs_timer timer;

    wsfs_timeout_manager_init(&manager);
    wsfs_timer_init(&timer, &manager);

    bool triggered = false;
    wsfs_timer_start(&timer, wsfs_timepoint_in_msec(250), &on_timeout, &triggered);
    usleep(500 * 1000);
    wsfs_timer_cancel(&timer);
    wsfs_timeout_manager_check(&manager);

    ASSERT_FALSE(triggered);
    
    wsfs_timer_cleanup(&timer);
    wsfs_timeout_manager_cleanup(&manager);
}

TEST(timer, multiple_timers)
{
    static size_t const count = 5;
    struct wsfs_timeout_manager manager;
    struct wsfs_timer timer[count];
    bool triggered[count];

    wsfs_timeout_manager_init(&manager);

    for(size_t i = 0; i < count; i++)
    {
        wsfs_timer_init(&timer[i], &manager);
        triggered[i] = false;
        wsfs_timer_start(&timer[i], wsfs_timepoint_in_msec(300 - (50 * i)), &on_timeout, &triggered[i]);
    }

    for(size_t i = 0; i < count; i++)
    {
        usleep(100 * 1000);
        wsfs_timeout_manager_check(&manager);
    }

    for(size_t i = 0; i < count; i++)
    {
        ASSERT_TRUE(triggered[i]);    
        wsfs_timer_cleanup(&timer[i]);
    }

    wsfs_timeout_manager_cleanup(&manager);
}
