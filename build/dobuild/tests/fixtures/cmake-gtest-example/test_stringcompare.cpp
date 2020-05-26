#include <gtest/gtest.h>

#include <string>
using std::string;

char const actualValTrue[] = "hello gtest";
char const actualValFalse[] = "hello world";
char const expectVal[] = "hello gtest";

TEST(StrCompare, Equal)
{
  EXPECT_STREQ(expectVal, actualValTrue);
}

TEST(StrCompare, NotEqual)
{
  EXPECT_STRNE(expectVal, actualValFalse);
}

TEST(StrCompare, WithNonReproducibleValues)
{
  EXPECT_STREQ(__DATE__, __DATE__);
  EXPECT_STREQ(__TIME__, __TIME__);
}
