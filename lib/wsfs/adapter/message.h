#ifndef WSFS_MESSAGE_H
#define WSFS_MESSAGE_H

#ifndef __cplusplus
#include <stddef.h>
#else
#include <cstddef>
using std::size_t;
#endif

struct wsfs_message
{
    struct wsfs_message * next;
    char * data;
    size_t length;
};

#ifdef __cplusplus
extern "C"
{
#endif

extern struct wsfs_message * wsfs_message_create(size_t length);

extern void wsfs_message_dispose(
    struct wsfs_message * message);

#ifdef __cplusplus
}
#endif

#endif
