#ifndef WF_MESSAGE_QUEUE_H
#define WF_MESSAGE_QUEUE_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

struct wf_message_queue;
struct wf_message;

struct wf_message_queue
{
    struct wf_message * first;
    struct wf_message * last;
};

#ifdef __cplusplus
extern "C"
{
#endif

extern void wf_message_queue_init(
    struct wf_message_queue * queue);

extern void wf_message_queue_cleanup(
    struct wf_message_queue * queue);

extern bool wf_message_queue_empty(
    struct wf_message_queue * queue);

extern void wf_message_queue_push(
    struct wf_message_queue * queue,
    struct wf_message * message);

extern struct wf_message * wf_message_queue_pop(
    struct wf_message_queue * queue);

#ifdef __cplusplus
}
#endif

#endif
