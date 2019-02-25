#ifndef WSFS_MESSAGE_QUEUE_H
#define WSFS_MESSAGE_QUEUE_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

struct wsfs_message_queue;
struct wsfs_message;

struct wsfs_message_queue
{
    struct wsfs_message * first;
    struct wsfs_message * last;
};

#ifdef __cplusplus
extern "C"
{
#endif

extern void wsfs_message_queue_init(
    struct wsfs_message_queue * queue);

extern void wsfs_message_queue_cleanup(
    struct wsfs_message_queue * queue);

extern bool wsfs_message_queue_empty(
    struct wsfs_message_queue * queue);

extern void wsfs_message_queue_push(
    struct wsfs_message_queue * queue,
    struct wsfs_message * message);

extern struct wsfs_message * wsfs_message_queue_pop(
    struct wsfs_message_queue * queue);

#ifdef __cplusplus
}
#endif

#endif
