#include <gtest/gtest.h>
#include <cstring>
#include "webfuse/core/message.h"

TEST(wf_message, create)
{
    json_t * value = json_object();
    
    struct wf_message * message = wf_message_create(value);
    ASSERT_NE(nullptr, message);
    ASSERT_EQ(2, message->length);
    ASSERT_TRUE(0 == strncmp("{}", message->data, 2));

    wf_message_dispose(message);
    json_decref(value);
}

TEST(wf_message, fail_to_create)
{
    struct wf_message * message = wf_message_create(nullptr);
    ASSERT_EQ(nullptr, message);
}