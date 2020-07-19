#ifndef WF_TEST_UTIL_INVOKATION_HANDLER_HPP
#define WF_TEST_UTIL_INVOKATION_HANDLER_HPP

#include <string>

struct wf_json;

namespace webfuse_test
{

class InvokationHandler
{
public:
    virtual ~InvokationHandler() = default;
    virtual std::string Invoke(char const * method, wf_json const * params) = 0;
};

}

#endif
