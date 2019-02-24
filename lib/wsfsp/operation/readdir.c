#include "wsfsp/operation/readdir_intern.h"
#include <stdio.h>

void wsfsp_readdir(
    struct wsfsp_invokation_context * context,
    json_t * params,
    int id)
{
    (void) context;
    (void) params;
    (void) id;

    puts("readdir");        
}

void wsfsp_respond_readdir(
    struct wsfsp_request * request,
    struct wsfsp_dirbuffer * dirbuffer)
{
    (void) request;
    (void) dirbuffer;

    // ToDo: implement me
}

