#include "wsfsp/operation/open_intern.h"
#include <stdio.h>
#include "wsfsp/operation/error.h"

void wsfsp_open(
    struct wsfsp_invokation_context * context,
    json_t * params,
    int id)
{
    (void) context;
    (void) params;
    (void) id;

    puts("open");        
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

void wsfsp_respond_open(
    struct wsfsp_request * request,
    uint32_t handle)
{
    (void) request;
    (void) handle;

    // ToDo: implement me
}
