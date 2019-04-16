#include "webfuse/core/message_queue.h"
#include "webfuse/core/message.h"
#include "webfuse/core/container_of.h"

void wf_message_queue_cleanup(
    struct wf_slist * queue)
{
    struct wf_slist_item * item = queue->first;
    while (NULL != item)
    {
        struct wf_slist_item * next = item->next;
        struct wf_message * message = WF_CONTAINER_OF(item, struct wf_message, item);
        wf_message_dispose(message);
        item = next;
    }
    wf_slist_init(queue);
}
