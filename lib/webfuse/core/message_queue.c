#include "webfuse/core/message_queue.h"
#include "webfuse/core/message.h"

void wf_message_queue_init(
    struct wf_message_queue * queue)
{
    queue->first = NULL;
    queue->last = NULL;
    
}

void wf_message_queue_cleanup(
    struct wf_message_queue * queue)
{
    struct wf_message * message = queue->first;
    while (NULL != message)
    {
        struct wf_message * next = message->next;
        wf_message_dispose(message);
        message = next;
    }
    wf_message_queue_init(queue);
}

bool wf_message_queue_empty(
    struct wf_message_queue * queue)
{
    return (NULL == queue->first);
}

void wf_message_queue_push(
    struct wf_message_queue * queue,
    struct wf_message * message)
{
    message->next = NULL;

    if (NULL != queue->last)
    {
        queue->last->next = message;
        queue->last = message;
    }
    else
    {
        queue->first = message;
        queue->last = message;
    }
}

struct wf_message * wf_message_queue_pop(
    struct wf_message_queue * queue)
{
    struct wf_message * const result = queue->first;
    if (NULL != result)
    {
        queue->first = queue->first->next;
        if (NULL == queue->first)
        {
            queue->last = NULL;
        }
    } 

    return result;
}
