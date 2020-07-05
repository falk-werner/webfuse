#include "webfuse/impl/util/buffer.h"
#include <gtest/gtest.h>

TEST(wf_buffer, append)
{
    wf_buffer buffer;
    wf_impl_buffer_init(&buffer, 2);

    wf_impl_buffer_append(&buffer, "Hello", 5);
    wf_impl_buffer_append(&buffer, ", "   , 2);
    wf_impl_buffer_append(&buffer, "World", 6);

    ASSERT_STREQ("Hello, World", wf_impl_buffer_data(&buffer));
    ASSERT_EQ(13, wf_impl_buffer_size(&buffer));

    wf_impl_buffer_cleanup(&buffer);
}

TEST(wf_buffer, is_empty)
{
    wf_buffer buffer;
    wf_impl_buffer_init(&buffer, 20);

    ASSERT_TRUE(wf_impl_buffer_is_empty(&buffer));

    wf_impl_buffer_append(&buffer, "Hi!", 4);
    ASSERT_FALSE(wf_impl_buffer_is_empty(&buffer));

    wf_impl_buffer_cleanup(&buffer);
}

TEST(wf_buffer, clear)
{
    wf_buffer buffer;
    wf_impl_buffer_init(&buffer, 2);

    ASSERT_TRUE(wf_impl_buffer_is_empty(&buffer));

    wf_impl_buffer_append(&buffer, "Hi!", 4);
    ASSERT_FALSE(wf_impl_buffer_is_empty(&buffer));

    wf_impl_buffer_clear(&buffer);
    ASSERT_TRUE(wf_impl_buffer_is_empty(&buffer));

    wf_impl_buffer_cleanup(&buffer);
}
