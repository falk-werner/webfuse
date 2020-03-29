#ifndef WF_JSONRPC_MOCK_TIMERMANAGER_HPP
#define WF_JSONRPC_MOCK_TIMERMANAGER_HPP

#include "webfuse/core/timer/timer.h"
#include "webfuse/core/timer/manager.h"
#include <gmock/gmock.h>

namespace wf_jsonrpc_test
{

class ITimer
{
public:
    virtual ~ITimer() = default;
    virtual wf_timer_manager * wf_timer_manager_create() = 0;
    virtual void wf_timer_manager_dispose(wf_timer_manager * manager) = 0;
    virtual void wf_timer_manager_check(wf_timer_manager * manager) = 0;
    virtual wf_timer * wf_timer_create(
        wf_timer_manager * manager,
        wf_timer_on_timer_fn * on_timer,
        void * user_data) = 0;
    virtual void wf_timer_dispose(wf_timer * timer) = 0;
    virtual void wf_timer_start(wf_timer * timer, int timeout_ms) = 0;
   virtual void wf_timer_cancel(wf_timer * timer) = 0; 
};

class MockTimer: public ITimer
{
public:
    MockTimer();
    ~MockTimer() override;
    MOCK_METHOD0(wf_timer_manager_create, wf_timer_manager * ());
    MOCK_METHOD1(wf_timer_manager_dispose, void(wf_timer_manager * manager));
    MOCK_METHOD1(wf_timer_manager_check, void (wf_timer_manager * manager));
    MOCK_METHOD3(wf_timer_create,  wf_timer *(
        wf_timer_manager * manager,
        wf_timer_on_timer_fn * on_timer,
        void * user_data));
    MOCK_METHOD1(wf_timer_dispose, void (wf_timer * timer));
    MOCK_METHOD2(wf_timer_start, void (wf_timer * timer, int timeout_ms));
    MOCK_METHOD1(wf_timer_cancel, void (wf_timer * timer)); 

};


}

#endif
