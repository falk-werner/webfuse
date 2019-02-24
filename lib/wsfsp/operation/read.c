#include "wsfsp/operation/read_intern.h"
#include <stdio.h>

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

void wsfsp_respond_read(
    struct wsfsp_request * request,
    char const * data,
    size_t length)
{
    (void) request;
    (void) data;
    (void) length;
}