#include "webfuse/filesystem/groupid.hpp"
#include <gtest/gtest.h>

using webfuse::group_id;

TEST(group_id, invalid)
{
    group_id invalid_group;

    ASSERT_EQ(group_id::invalid, invalid_group);
}

TEST(group_id, to_gid)
{
    group_id group(69);
    gid_t id = group.to_gid();

    ASSERT_EQ(69, id);
}

TEST(group_id, from_gid)
{
    gid_t id = 99;
    auto group = group_id::from_gid(id);

    ASSERT_EQ(99, group);
}
