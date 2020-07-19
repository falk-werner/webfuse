#ifndef WF_IMPL_MESSAGE_H
#define WF_IMPL_MESSAGE_H

#ifndef __cplusplus
#include <stddef.h>
#else
#include <cstddef>
using std::size_t;
#endif

#include "webfuse/impl/util/slist.h"

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

extern struct wf_message *
wf_impl_message_create(
    char * value,
    size_t length);

extern void
wf_impl_message_dispose(
    struct wf_message * message);

#ifdef __cplusplus
}
#endif

#endif
