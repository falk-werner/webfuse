#include <gtest/gtest.h> // googletest header file

#include <string>
using std::string;

char const actualValTrue[] = "hello gtest";
char const actualValFalse[] = "hello world";
char const expectVal[] = "hello gtest";

TEST(StrCompare, CStrEqual)
{
  EXPECT_STREQ(expectVal, actualValTrue);
}

TEST(StrCompare, CStrNotEqual)
{
  EXPECT_STRNE(expectVal, actualValFalse);
}
