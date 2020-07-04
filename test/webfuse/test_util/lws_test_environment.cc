#include <gtest/gtest.h>
#include "webfuse/impl/util/lws_log.h"

namespace webfuse_test
{

class LwsTestEnvironment: public testing::Environment
{
public:
    void SetUp()
    {
        wf_impl_lwslog_disable();
    }
};

::testing::Environment * const lws_env = ::testing::AddGlobalTestEnvironment(new LwsTestEnvironment());

}