#ifndef WSFSP_PROVIDER_DEFAULT_H
#define WSFSP_PROVIDER_DEFAULT_H

#include "wsfsp/provider.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern void wsfsp_lookup_default(
    struct wsfsp_request * request,
    ino_t parent,
    char const * name,
    void * user_data);

extern void wsfsp_getattr_default(
    struct wsfsp_request * request,
    ino_t inode,
    void * user_data);

extern void wsfsp_readdir_default(
    struct wsfsp_request * request,
    ino_t directory,
    void * user_data);

extern void wsfsp_open_default(
    struct wsfsp_request * request,
    ino_t inode,
    int flags,
    void * user_data);

extern void wsfsp_close_default(
    ino_t inode,
    uint32_t handle,
    int flags,
    void * user_data);

extern void wsfsp_read_default(
    struct wsfsp_request * request,
    ino_t inode,
    uint32_t handle,
    size_t offset,
    size_t length,
    void * user_data);

extern void wsfsp_connected_default(
    void * user_data);

extern void wsfsp_disconnected_default(
void * user_data);

extern void wsfsp_ontimer_default(
    void * user_data);


#ifdef __cplusplus
}
#endif

#endif
