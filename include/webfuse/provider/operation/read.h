////////////////////////////////////////////////////////////////////////////////
/// \file provider/operation/read.h
/// \brief Read contents of a file.
////////////////////////////////////////////////////////////////////////////////

#ifndef WFP_OPERATION_READ_H
#define WFP_OPERATION_READ_H

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

#include "webfuse/provider/api.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_request;

//------------------------------------------------------------------------------
/// \brief Requests content of a file.
///
/// On success, up to \arg length bytes should be returned via \ref
/// wfp_respond_read.
///
/// \note After this function is called, exactly one response must be sent,
///       either via \ref wfp_respond_read or via \ref wfp_respond_error.
/// 
/// \param request pointer to request
/// \param inode inode of the file to read
/// \param handle handle of the file to read (returned by open)
/// \param offset offset within the file where to start reading
/// \param length amount of bytes to read
/// \param user_data used defined context
///
/// \see wfp_respond_read
/// \see wfp_respond_error
//------------------------------------------------------------------------------
typedef void wfp_read_fn(
    struct wfp_request * request,
    ino_t inode,
    uint32_t handle,
    size_t offset,
    size_t length,
    void * user_data);

//------------------------------------------------------------------------------
/// \brief Respond to read.
///
/// \note The user is responsible to manage lifetime of \arg data.
///
/// \param request pointer to request
/// \param data data read from file
/// \param length amount of bytes read
//------------------------------------------------------------------------------
extern WFP_API void wfp_respond_read(
    struct wfp_request * request,
    char const * data,
    size_t length);

#ifdef __cplusplus
}
#endif

#endif
