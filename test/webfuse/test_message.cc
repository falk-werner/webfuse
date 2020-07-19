#include "webfuse/impl/message.h"

#include <cstring>
#include <cstdlib>
#include <libwebsockets.h>

#include <gtest/gtest.h>

TEST(wf_message, create)
{
    char * data = (char*) malloc(LWS_PRE + 2);
    data[LWS_PRE    ] = '{';
    data[LWS_PRE + 1] = '}';
    
    struct wf_message * message = wf_impl_message_create(&(data[LWS_PRE]), 2);
    ASSERT_NE(nullptr, message);
    ASSERT_EQ(2, message->length);
    ASSERT_TRUE(0 == strncmp("{}", message->data, 2));

    wf_impl_message_dispose(message);
}

