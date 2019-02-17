#include "wsfsp/provider.h"

void wsfsp_respond_error(
    struct wsfsp_request * request,
    int status)
{
    (void) request;
    (void) status;

    // ToDo: implement me
}

void wsfsp_respond_lookup(
    struct wsfsp_request * request,
    struct stat const * stat)
{
    (void) request;
    (void) stat;

    // ToDo: implement me
}

void wsfsp_respond_getattr(
    struct wsfsp_request * request,
    struct stat const * stat)
{
    (void) request;
    (void) stat;

    // ToDo: implement me
}

void wsfsp_respond_readdir(
    struct wsfsp_request * request,
    struct wsfsp_dirbuffer * dirbuffer)
{
    (void) request;
    (void) dirbuffer;

    // ToDo: implement me
}

void wsfsp_respond_open(
    struct wsfsp_request * request,
    uint32_t handle)
{
    (void) request;
    (void) handle;

    // ToDo: implement me
}

void wsfsp_respond_read(
    struct wsfsp_request * request,
    char const * data,
    size_t length)
{
    (void) request;
    (void) data;
    (void) length;

    // ToDo: implement me
}
