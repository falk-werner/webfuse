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
    ASSERT_EQ(&my_struct, wf_container_of(first, MyStruct, first));
}

TEST(ContainerOf, SecondMember)
{
    MyStruct my_struct = {23, 42};

    int * second = &my_struct.second;
    ASSERT_EQ(&my_struct, wf_container_of(second, MyStruct, second));
}