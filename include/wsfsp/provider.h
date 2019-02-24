#ifndef WSFSP_PROVIDER_H
#define WSFSP_PROVIDER_H

#include <wsfsp/operation/lookup.h>
#include <wsfsp/operation/getattr.h>
#include <wsfsp/operation/readdir.h>
#include <wsfsp/operation/open.h>
#include <wsfsp/operation/close.h>
#include <wsfsp/operation/read.h>

typedef void wsfsp_connected_fn(
    void * user_data);

typedef void wsfsp_disconnected_fn(
void * user_data);

typedef void wsfsp_ontimer_fn(
    void * user_data);

struct wsfsp_provider
{
    wsfsp_connected_fn * connected;
    wsfsp_disconnected_fn * disconnected;
    wsfsp_ontimer_fn * ontimer;
    wsfsp_lookup_fn * lookup;
    wsfsp_getattr_fn * getattr;
    wsfsp_readdir_fn * readdir;
    wsfsp_open_fn * open;
    wsfsp_close_fn * close;
    wsfsp_read_fn * read;
};


#endif
