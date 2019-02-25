#ifndef WSFS_PROVIDER_PROVIDER_H
#define WSFS_PROVIDER_PROVIDER_H

#include <wsfs/provider/operation/lookup.h>
#include <wsfs/provider/operation/getattr.h>
#include <wsfs/provider/operation/readdir.h>
#include <wsfs/provider/operation/open.h>
#include <wsfs/provider/operation/close.h>
#include <wsfs/provider/operation/read.h>

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
