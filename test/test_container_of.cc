#include <gtest/gtest.h>
#include "webfuse/core/container_of.h"

namespace
{

struct MyStruct
{
    int first;
    int second;
};

}

TEST(ContainerOf, FirstMember)
{
    MyStruct my_struct = {23, 42};

    int * first = &my_struct.first;
    ASSERT_EQ(&my_struct, WF_CONTAINER_OF(first, MyStruct, first));
}

TEST(ContainerOf, SecondMember)
{
    MyStruct my_struct = {23, 42};

    int * second = &my_struct.second;
    ASSERT_EQ(&my_struct, WF_CONTAINER_OF(second, MyStruct, second));
}