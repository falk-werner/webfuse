#include <gtest/gtest.h>
#include <jansson.h>

namespace webfuse_test
{

class JanssonTestEnvironment: public ::testing::Environment
{
public:
    void SetUp()
    {
        json_object_seed(0);
    }
};
#
}