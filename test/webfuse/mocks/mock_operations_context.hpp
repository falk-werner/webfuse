#ifndef MOCK_OPERATIONS_CONTEXT_HPP
#define MOCK_OPERATIONS_CONTEXT_HPP

#include "webfuse/adapter/impl/operations.h"
#include <gmock/gmock.h>

namespace webfuse_test
{

class MockOperationsContext
{
public:
    MockOperationsContext();
    virtual ~MockOperationsContext();
    MOCK_METHOD1(wf_impl_operations_context_get_proxy, wf_jsonrpc_proxy * (
	    struct wf_impl_operations_context * context));

};

}

#endif
