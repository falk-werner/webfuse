#include "wsfs/adapter/message_queue.h"
#include "wsfs/adapter/message.h"

void wsfs_message_queue_init(
    struct wsfs_message_queue * queue)
{
    queue->first = NULL;
    queue->last = NULL;
    
}

void wsfs_message_queue_cleanup(
    struct wsfs_message_queue * queue)
{
    struct wsfs_message * message = queue->first;
    while (NULL != message)
    {
        struct wsfs_message * next = message->next;
        wsfs_message_dispose(message);
        message = next;
    }
    wsfs_message_queue_init(queue);
}

bool wsfs_message_queue_empty(
    struct wsfs_message_queue * queue)
{
    return (NULL == queue->first);
}

void wsfs_message_queue_push(
    struct wsfs_message_queue * queue,
    struct wsfs_message * message)
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

struct wsfs_message * wsfs_message_queue_pop(
    struct wsfs_message_queue * queue)
{
    struct wsfs_message * const result = queue->first;
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
