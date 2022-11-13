#include "webfuse/filesystem/userid.hpp"
#include <gtest/gtest.h>

using webfuse::user_id;

TEST(user_id, invalid)
{
    user_id invalid_user;

    ASSERT_EQ(user_id::invalid, invalid_user);
}

TEST(user_id, to_uid)
{
    user_id user(42);
    uid_t id = user.to_uid();

    ASSERT_EQ(42, id);
}

TEST(user_id, from_uid)
{
    uid_t id = 23;
    auto user = user_id::from_uid(id);

    ASSERT_EQ(23, user);
}
