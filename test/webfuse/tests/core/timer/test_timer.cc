#include <gtest/gtest.h>

#include <cstddef>
#include <chrono>
#include <thread>

#include "webfuse/core/timer/timer.h"
#include "webfuse/core/timer/manager.h"

using std::size_t;
using namespace std::chrono_literals;

extern "C"
{
    void on_timeout(struct wf_timer * timer, void * user_data)
    {
        (void) timer;

        bool * triggered = reinterpret_cast<bool*>(user_data);
        *triggered = true;
    }
}

TEST(wf_timer, init)
{
    bool triggered = false;
    struct wf_timer_manager * manager = wf_timer_manager_create();
    struct wf_timer * timer = wf_timer_create(manager, &on_timeout, reinterpret_cast<void*>(&triggered));

    wf_timer_dispose(timer);
    wf_timer_manager_dispose(manager);
}

TEST(wf_timer, trigger)
{
    bool triggered = false;
    struct wf_timer_manager * manager = wf_timer_manager_create();
    struct wf_timer * timer = wf_timer_create(manager, &on_timeout, reinterpret_cast<void*>(&triggered));

    wf_timer_start(timer, -1);
    wf_timer_manager_check(manager);

    ASSERT_TRUE(triggered);

    wf_timer_dispose(timer);
    wf_timer_manager_dispose(manager);
}

TEST(wf_timer, trigger_on_dispose)
{
    bool triggered = false;
    struct wf_timer_manager * manager = wf_timer_manager_create();
    struct wf_timer * timer = wf_timer_create(manager, &on_timeout, reinterpret_cast<void*>(&triggered));

    wf_timer_start(timer, (5 * 60 * 1000));

    wf_timer_manager_dispose(manager);
    ASSERT_TRUE(triggered);

    wf_timer_dispose(timer);
}

TEST(wf_timer, cancel)
{
    bool triggered = false;
    struct wf_timer_manager * manager = wf_timer_manager_create();
    struct wf_timer * timer = wf_timer_create(manager, &on_timeout, reinterpret_cast<void*>(&triggered));

    wf_timer_start(timer, 250);
    std::this_thread::sleep_for(500ms);
    wf_timer_cancel(timer);
    wf_timer_manager_check(manager);

    ASSERT_FALSE(triggered);
    
    wf_timer_dispose(timer);
    wf_timer_manager_dispose(manager);
}

TEST(wf_timer, cancel_multiple_timers)
{
    static size_t const count = 5;
    struct wf_timer_manager * manager = wf_timer_manager_create();
    struct wf_timer * timer[count];

    bool triggered = false;
    for(size_t i = 0; i < count; i++)
    {
        timer[i] = wf_timer_create(manager, &on_timeout, reinterpret_cast<void*>(&triggered));
        wf_timer_start(timer[i], 0);
    }

    std::this_thread::sleep_for(10ms);
    for(size_t i = 0; i < count; i++)
    {
        wf_timer_cancel(timer[i]);
    }

    wf_timer_manager_check(manager);
    ASSERT_FALSE(triggered);
    
    for(size_t i = 0; i < count; i++)
    {
        wf_timer_dispose(timer[i]);
    }
    wf_timer_manager_dispose(manager);
}

TEST(wf_timer, multiple_timers)
{
    static size_t const count = 5;
    struct wf_timer_manager * manager = wf_timer_manager_create();
    struct wf_timer * timer[count];
    bool triggered[count];

    for(size_t i = 0; i < count; i++)
    {
        timer[i] = wf_timer_create(manager, &on_timeout, reinterpret_cast<void*>(&triggered[i]));
        triggered[i] = false;
        wf_timer_start(timer[i], (300 - (50 * i)));
    }

    for(size_t i = 0; i < count; i++)
    {
        std::this_thread::sleep_for(100ms);
        wf_timer_manager_check(manager);
    }

    for(size_t i = 0; i < count; i++)
    {
        ASSERT_TRUE(triggered[i]);    
        wf_timer_dispose(timer[i]);
    }

    wf_timer_manager_dispose(manager);
}

TEST(wf_timer, dont_trigger_null_callback)
{
    struct wf_timer_manager * manager = wf_timer_manager_create();
    struct wf_timer * timer = wf_timer_create(manager, nullptr, nullptr);
    
    wf_timer_start(timer, -1);
    wf_timer_manager_check(manager);

    wf_timer_dispose(timer);
    wf_timer_manager_dispose(manager);
}
