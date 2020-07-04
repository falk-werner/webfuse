#ifndef WF_TEST_UTIL_INVOKATION_HANDLER_HPP
#define WF_TEST_UTIL_INVOKATION_HANDLER_HPP

#include <jansson.h>
#include <string>

namespace webfuse_test
{

class InvokationHandler
{
public:
    virtual ~InvokationHandler() = default;
    virtual std::string Invoke(char const * method, json_t * params) = 0;
};

}

#endif
