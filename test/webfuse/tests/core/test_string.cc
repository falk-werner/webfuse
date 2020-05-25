#include <gtest/gtest.h>
#include <stdlib.h>

#include "webfuse/core/string.h"

TEST(wf_string_create, Default)
{
    char * value = wf_create_string("test %s/%d", "hello", 42);
    ASSERT_STREQ("test hello/42", value);
    free(value);
}

TEST(wf_string_create, EmptyString)
{
    char * value = wf_create_string("");
    ASSERT_STREQ("", value);
    free(value);
}
