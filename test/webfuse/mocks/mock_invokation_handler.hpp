#ifndef WF_MOCK_INVOKATION_HANDLER_HPP
#define WF_MOCK_INVOKATION_HANDLER_HPP

#include "webfuse/utils/ws_server2.hpp"
#include <gtest/gtest.h>

namespace webfuse_test
{

class MockInvokationHander: public IIvokationHandler
{
public:
    MOCK_METHOD2(Invoke, std::string(char const * method, json_t * params));
};

}

#endif
