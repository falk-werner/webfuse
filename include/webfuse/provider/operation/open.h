////////////////////////////////////////////////////////////////////////////////
/// \file provider/operation/open.h
/// \brief Open a file.
////////////////////////////////////////////////////////////////////////////////

#ifndef WFP_OPERATION_OPEN_H
#define WFP_OPERATION_OPEN_H

#ifndef __cplusplus
#include <inttypes.h>
#else
#include <cinttypes>
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
/// \brief Open a file.
///
/// \note After this function is called, exactly one response must be sent,
///       either via \ref wfp_respond_open or via \ref wfp_respond_error.
///
/// \param request pointer to request
/// \param inode inode of the file to open
/// \param flags file open flags
/// \param user_data user defined context
///
/// \see wfp_respond_open
/// \see wfp_respond_error
//------------------------------------------------------------------------------
typedef void wfp_open_fn(
    struct wfp_request * request,
    ino_t inode,
    int flags,
    void * user_data);

//------------------------------------------------------------------------------
/// \brief Respond to open file.
///
/// \param request pointer to request
/// \param handle handle of the opened file
//------------------------------------------------------------------------------
extern WFP_API void wfp_respond_open(
    struct wfp_request * request,
    uint32_t handle);


#ifdef __cplusplus
}
#endif

#endif
