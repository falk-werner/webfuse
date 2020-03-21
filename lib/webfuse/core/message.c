#include "webfuse/core/message.h"

#include <stdlib.h>
#include <libwebsockets.h>

extern struct wf_message * wf_message_create(json_t const * value)
{
    struct wf_message * message = NULL;
    size_t const length = json_dumpb(value, NULL, 0, JSON_COMPACT);

    if (0 < length)
    {
        char * data = malloc(sizeof(struct wf_message) + LWS_PRE + length);
        message = (struct wf_message *) data;
        message->data = &data[sizeof(struct wf_message) + LWS_PRE];
        message->length = length;

        json_dumpb(value, message->data, length, JSON_COMPACT);
    }

    return message;
}

void wf_message_dispose(
    struct wf_message * message)
{
    free(message);    
}
