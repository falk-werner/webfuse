#include "wsfsp/provider_default.h"
#include "wsfsp/operation/error.h"

void wsfsp_lookup_default(
    struct wsfsp_request * request,
    ino_t parent,
    char const * name,
    void * user_data)
{
    (void) parent;
    (void) name;
    (void) user_data;
    
    wsfsp_respond_error(request, -1);
}

void wsfsp_getattr_default(
    struct wsfsp_request * request,
    ino_t inode,
    void * user_data)
{
    (void) inode;
    (void) user_data;

    wsfsp_respond_error(request, -1);
}

void wsfsp_readdir_default(
    struct wsfsp_request * request,
    ino_t directory,
    void * user_data)
{
    (void) directory;
    (void) user_data;

    wsfsp_respond_error(request, -1);
}

void wsfsp_open_default(
    struct wsfsp_request * request,
    ino_t inode,
    int flags,
    void * user_data)
{
    (void) inode;
    (void) flags;
    (void) user_data;

    wsfsp_respond_error(request, -1);
}

void wsfsp_close_default(
    ino_t inode,
    uint32_t handle,
    int flags,
    void * user_data)
{
    (void) inode;
    (void) handle;
    (void) flags;
    (void) user_data;

    // empty
}

void wsfsp_read_default(
    struct wsfsp_request * request,
    ino_t inode,
    uint32_t handle,
    size_t offset,
    size_t length,
    void * user_data)
{
    (void) inode;
    (void) handle;
    (void) offset;
    (void) length;
    (void) user_data;

    wsfsp_respond_error(request, -1);
}

void wsfsp_connected_default(
    void * user_data)
{
    (void) user_data;

    // empty
}

void wsfsp_disconnected_default(
    void * user_data)
{
    (void) user_data;

    // empty
}

void wsfsp_ontimer_default(
    void * user_data)
{
    (void) user_data;

    // empty
}
