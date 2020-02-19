////////////////////////////////////////////////////////////////////////////////
/// \file provider/static_filesystem.h
/// \brief Reference implementation of static filesystem.
///
/// This header is used by integration tests. It may be removed from the 
/// library.
///
/// \todo Remove this header from library
////////////////////////////////////////////////////////////////////////////////

#ifndef WFP_STATIC_FILESYSTEM_H
#define WFP_STATIC_FILESYSTEM_H

#ifndef __cplusplus
#include <stddef.h>
#else
#include <cstddef>
using ::std::size_t;
#endif

#include <webfuse/provider/api.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_client_config;

//------------------------------------------------------------------------------
/// \deprecated This will be removed. Dont use it.
//------------------------------------------------------------------------------
struct wfp_static_filesystem;

//------------------------------------------------------------------------------
/// \deprecated This will be removed. Dont use it.
//------------------------------------------------------------------------------
typedef size_t
wfp_static_filesystem_read_fn(
    size_t offset,
    char * buffer,
    size_t buffer_size,
    void * user_data);

//------------------------------------------------------------------------------
/// \deprecated This will be removed. Dont use it.
//------------------------------------------------------------------------------
typedef void
wfp_static_filesystem_get_info_fn(
    void * user_data,
    int * result_mode,
    size_t * result_size);


//------------------------------------------------------------------------------
/// \deprecated This will be removed. Dont use it.
//------------------------------------------------------------------------------
extern WFP_API struct wfp_static_filesystem *
wfp_static_filesystem_create(
    struct wfp_client_config * config);

//------------------------------------------------------------------------------
/// \deprecated This will be removed. Dont use it.
//------------------------------------------------------------------------------
extern WFP_API void
wfp_static_filesystem_dispose(
    struct wfp_static_filesystem * filesystem);

//------------------------------------------------------------------------------
/// \deprecated This will be removed. Dont use it.
//------------------------------------------------------------------------------
extern WFP_API void
wfp_static_filesystem_add(
    struct wfp_static_filesystem * filesystem,
    char const * path,
    int mode,
    char const * content,
    size_t length);

//------------------------------------------------------------------------------
/// \deprecated This will be removed. Dont use it.
//------------------------------------------------------------------------------
extern WFP_API void
wfp_static_filesystem_add_text(
    struct wfp_static_filesystem * filesystem,
    char const * path,
    int mode,
    char const * content);

//------------------------------------------------------------------------------
/// \deprecated This will be removed. Dont use it.
//------------------------------------------------------------------------------
extern WFP_API void
wfp_static_filesystem_add_file(
    struct wfp_static_filesystem * filesystem,
    char const * path,
    char const * filename);

//------------------------------------------------------------------------------
/// \deprecated This will be removed. Dont use it.
//------------------------------------------------------------------------------
extern WFP_API void
wfp_static_filesystem_add_generic(
    struct wfp_static_filesystem * filesystem,
    char const * path,
    wfp_static_filesystem_read_fn * read,
    wfp_static_filesystem_get_info_fn * get_info,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif
