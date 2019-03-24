#include "wsfs/core/message.h"

#include <stdlib.h>
#include <libwebsockets.h>

extern struct wsfs_message * wsfs_message_create(json_t const * value)
{
    struct wsfs_message * message = NULL;
    size_t const length = json_dumpb(value, NULL, 0, JSON_COMPACT);

    if (0 < length)
    {
        char * data = malloc(sizeof(struct wsfs_message) + LWS_PRE + length);
        message = (struct wsfs_message *) data;
        if (NULL != message)
        {
            message->data = &data[sizeof(struct wsfs_message) + LWS_PRE];
            message->length = length;
            message->next = NULL;

            json_dumpb(value, message->data, length, JSON_COMPACT);
        }
    }

    return message;
}

void wsfs_message_dispose(
    struct wsfs_message * message)
{
    free(message);    
}
