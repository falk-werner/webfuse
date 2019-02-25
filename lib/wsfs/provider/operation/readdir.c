#include "wsfs/provider/operation/readdir_intern.h"
#include <stdio.h>
#include "wsfs/provider/operation/error.h"

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

void wsfsp_readdir_default(
    struct wsfsp_request * request,
    ino_t directory,
    void * user_data)
{
    (void) directory;
    (void) user_data;

    wsfsp_respond_error(request, -1);
}

void wsfsp_respond_readdir(
    struct wsfsp_request * request,
    struct wsfsp_dirbuffer * dirbuffer)
{
    (void) request;
    (void) dirbuffer;

    // ToDo: implement me
}

