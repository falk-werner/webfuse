////////////////////////////////////////////////////////////////////////////////
/// \file provider/operation/getattr.h
/// \brief Get file attributes.
////////////////////////////////////////////////////////////////////////////////

#ifndef WFP_OPERATION_GETATTR_H
#define WFP_OPERATION_GETATTR_H

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
/// \brief Get file attributes.
///
/// \note After this function is called, exactly one response must be sent,
///       either via \ref wfp_respond_getattr or via \ref wfp_respond_error.
///
/// \param request pointer to request
/// \param inode inode of file to get attributes
/// \param user_data user defined context
///
/// \see wfp_respond_getattr
/// \see wfp_respond_error
//------------------------------------------------------------------------------
typedef void wfp_getattr_fn(
    struct wfp_request * request,
    ino_t inode,
    void * user_data);

//------------------------------------------------------------------------------
/// \brief Respond to a get attributes request.
///
/// \param request pointer to request
/// \param stat file attributes
//------------------------------------------------------------------------------
extern WFP_API void wfp_respond_getattr(
    struct wfp_request * request,
    struct stat const * stat);


#ifdef __cplusplus
}
#endif

#endif
