#include "wsfsp/operation/lookup_intern.h"
#include <stdio.h>

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

