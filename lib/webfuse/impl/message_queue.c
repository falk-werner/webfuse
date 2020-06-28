#include "webfuse/impl/message_queue.h"
#include "webfuse/impl/message.h"
#include "webfuse/impl/util/container_of.h"

void wf_impl_message_queue_cleanup(
    struct wf_slist * queue)
{
    struct wf_slist_item * item = wf_impl_slist_first(queue);
    while (NULL != item)
    {
        struct wf_slist_item * next = item->next;
        struct wf_message * message = wf_container_of(item, struct wf_message, item);
        wf_impl_message_dispose(message);
        item = next;
    }
    wf_impl_slist_init(queue);
}
