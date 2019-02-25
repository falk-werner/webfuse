#include "wsfs/adapter/message.h"

#include <stdlib.h>
#include <libwebsockets.h>

struct wsfs_message * wsfs_message_create(size_t length)
{
    char * data = malloc(sizeof(struct wsfs_message) + LWS_PRE + length);
    struct wsfs_message * message = (struct wsfs_message *) data;
    if (NULL != message)
    {
        message->data = &data[sizeof(struct wsfs_message) + LWS_PRE];
        message->length = length;
        message->next = NULL;
    }

    return message;
}

void wsfs_message_dispose(
    struct wsfs_message * message)
{
    free(message);    
}
