#ifndef WF_MESSAGE_QUEUE_H
#define WF_MESSAGE_QUEUE_H

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_slist;

extern void wf_message_queue_cleanup(
    struct wf_slist * queue);


#ifdef __cplusplus
}
#endif

#endif
