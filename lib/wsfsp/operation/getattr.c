#include "wsfsp/operation/getattr_intern.h"
#include <stdio.h>

#include "wsfsp/operation/error.h"


void wsfsp_getattr(
    struct wsfsp_invokation_context * context,
    json_t * params,
    int id)
{
    (void) context;
    (void) params;
    (void) id;

    puts("getattr");        
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

void wsfsp_respond_getattr(
    struct wsfsp_request * request,
    struct stat const * stat)
{
    (void) request;
    (void) stat;

    // ToDo: implement me
}
