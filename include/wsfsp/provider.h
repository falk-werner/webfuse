#ifndef _WSFSp_H
#define _WSFSp_H

#ifndef __cplusplus
#include <stddef.h>
#include <inttypes.h>
#else
#include <cstddef>
#include <cinttypes>
using std::size_t;
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


struct wsfsp_request;
struct wsfs_dirbuffer;

typedef void wsfsp_lookup_fn(
    struct wsfsp_request * request,
    ino_t parent,
    char const * name,
    void * user_data);

typedef void wsfsp_getattr_fn(
    struct wsfsp_request * request,
    ino_t inode,
    void * user_data);

typedef void wsfsp_readdir_fn(
    struct wsfsp_request * request,
    ino_t directory,
    struct wsfsp_dirbuffer * dirbuffer,
    void * user_data);

typedef void wsfsp_open_fn(
    struct wsfsp_request * request,
    ino_t inode,
    int flags,
    void * user_data);

typedef void wsfsp_close_fn(
    ino_t inode,
    uint32_t handle,
    int flags,
    void * user_data);

typedef void wsfsp_read_fn(
    struct wsfsp_request * request,
    ino_t inode,
    uint32_t handle,
    size_t offset,
    size_t length,
    void * user_data);

typedef void wsfsp_connected_fn(
    void * user_data);

typedef void wsfsp_disconnected_fn(
void * user_data);

typedef void wsfsp_ontimer_fn(
    void * user_data);

struct wsfsüp
{
    wsfsp_connected_fn * connected;
    wsfsp_disconnected_fn * connected;
    wsfsp_ontimer_fn * ontimer;
    wsfsp_lookup_fn * lookup;
    wsfsp_getattr_fn * getattr;
    wsfsp_readdir_fn * readdir;
    wsfsp_open_fn * open;
    wsfsp_close_fn * close;
    wsfsp_read_fn * read;
};

#ifdef __cplusplus
extern "C"
{
#endif

extern void wsfsp_respond_error(
    struct wsfsp_request * request,
    int status);

extern void wsfsp_respond_lookup(
    struct wsfsp_request * request,
    struct wsfs_stat const * stat);

extern void wsfsp_respond_getattr(
    struct wsfsp_request * request,
    struct stat const * stat);

extern void wsfsp_respond_readdir(
    struct wsfsp_request * request,
    struct wsfsp_dirbuffer * dirbuffer);

extern void wsfsp_respond_open(
    struct wsfsp_request * request,
    uint32_t handle);

extern void wsfsp_respond_read(
    struct wsfsp_request * request,
    char const * data,
    size_t length);

#ifdef __cplusplus
}
#endif

#endif
