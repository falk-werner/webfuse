#include "webfuse/provider/impl/dirbuffer.h"
#include <gtest/gtest.h>

TEST(DirBuffer, CreateDispose)
{
    wfp_dirbuffer * buffer = wfp_impl_dirbuffer_create();
    wfp_impl_dirbuffer_dispose(buffer);
}

TEST(DirBuffer, Add)
{
    wfp_dirbuffer * buffer = wfp_impl_dirbuffer_create();
    wfp_impl_dirbuffer_add(buffer, "answer", 42);

    ASSERT_EQ(1, json_array_size(buffer->entries));

    json_t * entry = json_array_get(buffer->entries, 0);
    ASSERT_STREQ("answer", json_string_value(json_object_get(entry, "name")));
    ASSERT_EQ(42, json_integer_value(json_object_get(entry, "inode")));

    wfp_impl_dirbuffer_dispose(buffer);
}

TEST(DirBuffer, Take)
{
    wfp_dirbuffer * buffer = wfp_impl_dirbuffer_create();
    json_t * entries = wfp_impl_dirbuffer_take(buffer);
    wfp_impl_dirbuffer_dispose(buffer);

    ASSERT_TRUE(json_is_array(entries));
    json_decref(entries);
}
