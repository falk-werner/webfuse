#ifndef MOCK_JSONRPC_PROXY_HPP
#define MOCK_JSONRPC_PROXY_HPP

#include "webfuse/core/jsonrpc/proxy_intern.h"
#include <gmock/gmock.h>

namespace webfuse_test
{

class MockJsonRpcProxy
{
public:
    MockJsonRpcProxy();
    virtual ~MockJsonRpcProxy();
    MOCK_METHOD6(wf_jsonrpc_proxy_vinvoke, void (
        struct wf_jsonrpc_proxy * proxy,
        wf_jsonrpc_proxy_finished_fn * finished,
        void * user_data,
        char const * method_name,
        char const * param_info,
        va_list args));

};

}

#endif
