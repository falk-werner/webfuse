////////////////////////////////////////////////////////////////////////////////
/// \file provider/operation/readdir.h
/// \brief List directory contents.
////////////////////////////////////////////////////////////////////////////////

#ifndef WFP_OPERATION_READDIR_H
#define WFP_OPERATION_READDIR_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "webfuse/provider/api.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wfp_dirbuffer;
struct wfp_request;

//------------------------------------------------------------------------------
/// \brief Requests the contents of a directory.
///
/// \note After this function is called, exactly one response must be sent,
///       either via \ref wfp_respond_readdir or via \ref wfp_respond_error.
///
/// \param request pointer to request
/// \param directory inode of directory to list
/// \param user_data user defined context
///
/// \see wfp_respond_readdir
/// \see wfp_respond_error
//------------------------------------------------------------------------------
typedef void wfp_readdir_fn(
    struct wfp_request * request,
    ino_t directory,
    void * user_data);

//------------------------------------------------------------------------------
/// \brief Respond to list directory contents.
///
/// \note The user is responsible to manage dirbuffe, p.e. to dispose
///       it after this function is called.
///
/// \param request pointer to request
/// \param dirbuffer contains contents of directory
//------------------------------------------------------------------------------
extern WFP_API void wfp_respond_readdir(
    struct wfp_request * request,
    struct wfp_dirbuffer * dirbuffer);

#ifdef __cplusplus
}
#endif

#endif
