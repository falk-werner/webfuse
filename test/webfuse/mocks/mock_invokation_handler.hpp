#ifndef WF_MOCK_INVOKATION_HANDLER_HPP
#define WF_MOCK_INVOKATION_HANDLER_HPP

#include "webfuse/test_util/invokation_handler.hpp"
#include <gmock/gmock.h>

namespace webfuse_test
{

class MockInvokationHander: public InvokationHandler
{
public:
    MOCK_METHOD2(Invoke, std::string(char const * method, wf_json const * params));
};

}

#endif
