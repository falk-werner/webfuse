#include "webfuse/impl/message_queue.h"
#include "webfuse/impl/message.h"
#include "webfuse/impl/util/slist.h"

#include <gtest/gtest.h>

#include <libwebsockets.h>
#include <cstring>
#include <cstdlib>

namespace
{

    struct wf_slist_item * create_message(char const * content)
    {
        std::string json = std::string("{\"content\": \"") + content + "\"}";
        char * data = (char*) malloc(LWS_PRE + json.size());
        memcpy(&(data[LWS_PRE]), json.c_str(), json.size());
        struct wf_message * message = wf_impl_message_create(&(data[LWS_PRE]), json.size());

        return &message->item;
    }

}

TEST(wf_message_queue, cleanup_empty_list)
{
    struct wf_slist queue;
    wf_impl_slist_init(&queue);

    wf_impl_message_queue_cleanup(&queue);
    ASSERT_TRUE(wf_impl_slist_empty(&queue));
}

TEST(wf_message_queue, cleanup_one_element)
{
    struct wf_slist queue;
    wf_impl_slist_init(&queue);

    wf_impl_slist_append(&queue, create_message("Hello"));

    wf_impl_message_queue_cleanup(&queue);
    ASSERT_TRUE(wf_impl_slist_empty(&queue));
}

TEST(wf_message_queue, cleanup_multiple_element)
{
    struct wf_slist queue;
    wf_impl_slist_init(&queue);

    wf_impl_slist_append(&queue, create_message("Hello"));
    wf_impl_slist_append(&queue, create_message("World"));
    wf_impl_slist_append(&queue, create_message("!"));

    wf_impl_message_queue_cleanup(&queue);
    ASSERT_TRUE(wf_impl_slist_empty(&queue));
}