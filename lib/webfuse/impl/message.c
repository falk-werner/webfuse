#include "webfuse/impl/message.h"

#include <stdlib.h>
#include <libwebsockets.h>

extern struct wf_message *
wf_impl_message_create(
    char * data,
    size_t length)
{
    struct wf_message * message = malloc(sizeof(struct wf_message));
    message->data = data;
    message->length = length;

    return message;
}

void
wf_impl_message_dispose(
    struct wf_message * message)
{
    char * raw_data = message->data - LWS_PRE;
    free(raw_data);
    free(message);    
}
