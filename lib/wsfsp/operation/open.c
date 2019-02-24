#include "wsfsp/operation/open_intern.h"
#include <stdio.h>

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

void wsfsp_respond_open(
    struct wsfsp_request * request,
    uint32_t handle)
{
    (void) request;
    (void) handle;

    // ToDo: implement me
}
