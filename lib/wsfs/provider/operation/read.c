#include "wsfs/provider/operation/read_intern.h"
#include <stdio.h>
#include "wsfs/provider/operation/error.h"
#include "wsfs/util.h"

void wsfsp_read(
    struct wsfsp_invokation_context * context,
    json_t * params,
    int id)
{
    (void) context;
    (void) params;
    (void) id;

    puts("read");  
}

void wsfsp_read_default(
    struct wsfsp_request * request,
    ino_t WSFS_UNUSED_PARAM(inode),
    uint32_t WSFS_UNUSED_PARAM(handle),
    size_t WSFS_UNUSED_PARAM(offset),
    size_t WSFS_UNUSED_PARAM(length),
    void * WSFS_UNUSED_PARAM(user_data))
{
    wsfsp_respond_error(request, -1);
}

void wsfsp_respond_read(
    struct wsfsp_request * request,
    char const * data,
    size_t length)
{
    (void) request;
    (void) data;
    (void) length;
}