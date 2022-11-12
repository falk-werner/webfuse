#include "webfuse/webfuse.hpp"
#include <gtest/gtest.h>

TEST(app, init)
{
    char args0[] = "webfuse";
    char * args[] = { args0, nullptr };
    webfuse::app(1, args);
}