#ifndef WF_MESSAGE_H
#define WF_MESSAGE_H

#ifndef __cplusplus
#include <stddef.h>
#else
#include <cstddef>
using std::size_t;
#endif

#include <jansson.h>
#include "webfuse/core/slist.h"

struct wf_message
{
    struct wf_slist_item item;
    char * data;
    size_t length;
};

#ifdef __cplusplus
extern "C"
{
#endif

extern struct wf_message * wf_message_create(
    json_t const * value);

extern void wf_message_dispose(
    struct wf_message * message);

#ifdef __cplusplus
}
#endif

#endif
