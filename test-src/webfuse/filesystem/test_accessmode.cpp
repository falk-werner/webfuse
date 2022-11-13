#include "webfuse/filesystem/accessmode.hpp"
#include <gtest/gtest.h>

using webfuse::access_mode;

TEST(accessmode, f_ok)
{
    ASSERT_EQ(0, access_mode::f_ok);
    ASSERT_EQ(F_OK, access_mode::f_ok);
}

class accessmode_test: public testing::TestWithParam<int> { };

TEST_P(accessmode_test, conversion)
{
    int const expected = GetParam();
    auto mode = access_mode::from_int(expected);
    ASSERT_EQ(expected, mode.to_int());
}

INSTANTIATE_TEST_CASE_P(accesmode_values, accessmode_test, 
    testing::Values(
        F_OK, R_OK, W_OK, X_OK, 
        R_OK | W_OK, R_OK | X_OK, W_OK | X_OK, 
        R_OK | W_OK | X_OK)
);
