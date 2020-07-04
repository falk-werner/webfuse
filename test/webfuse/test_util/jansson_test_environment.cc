#include <gtest/gtest.h>
#include <jansson.h>

namespace webfuse_test
{

class JanssonTestEnvironment: public ::testing::Environment
{
public:
    ~JanssonTestEnvironment() override { }

    void SetUp() override
    {
        json_object_seed(0);
    }
};

::testing::Environment * const jansson_env = ::testing::AddGlobalTestEnvironment(new JanssonTestEnvironment());

}