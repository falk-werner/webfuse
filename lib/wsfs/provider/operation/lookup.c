#include "wsfs/provider/operation/lookup_intern.h"
#include <stdio.h>
#include "wsfs/provider/operation/error.h"

void wsfsp_lookup(
    struct wsfsp_invokation_context * context,
    json_t * params,
    int id)
{
    (void) context;
    (void) params;
    (void) id;

    puts("lookup");    
}

void wsfsp_respond_lookup(
    struct wsfsp_request * request,
    struct stat const * stat)
{
    (void) request;
    (void) stat;

}

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

