////////////////////////////////////////////////////////////////////////////////
/// \file provider/operation/lookup.h
/// \brief Lookup file.
////////////////////////////////////////////////////////////////////////////////

#ifndef WFP_OPERATION_LOOKUP_H
#define WFP_OPERATION_LOOKUP_H

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
/// \brief Lookup a file or directory.
///
/// \note After this function is called, exactly one response must be sent,
///       either via \ref wfp_respond_lookup or via \ref wfp_respond_error.
///
/// \param request pointer to request
/// \param parent inode of parent
/// \param name   name of the filesystem object to lookup
/// \param user_data pointer to user defined context
///
/// \see wfp_respond_lookup
/// \see wfp_respond_error
//------------------------------------------------------------------------------
typedef void wfp_lookup_fn(
    struct wfp_request * request,
    ino_t parent,
    char const * name,
    void * user_data);

//------------------------------------------------------------------------------
/// \brief Respond to lookup request.
///
/// \param request pointer to request
/// \param stat attributes of filesystem object
//------------------------------------------------------------------------------
extern WFP_API void wfp_respond_lookup(
    struct wfp_request * request,
    struct stat const * stat);


#ifdef __cplusplus
}
#endif

#endif
