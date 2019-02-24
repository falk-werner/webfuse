#include "wsfsp/operation/read_intern.h"
#include <stdio.h>
#include "wsfsp/operation/error.h"

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

void wsfsp_respond_read(
    struct wsfsp_request * request,
    char const * data,
    size_t length)
{
    (void) request;
    (void) data;
    (void) length;
}