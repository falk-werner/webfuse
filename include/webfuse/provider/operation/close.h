////////////////////////////////////////////////////////////////////////////////
/// \file provider/operation/close.h
/// \brief Provider's close callback.
////////////////////////////////////////////////////////////////////////////////

#ifndef WFP_OPERATION_CLOSE_H
#define WFP_OPERATION_CLOSE_H

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

//------------------------------------------------------------------------------
/// \brief Callback invoked when a file is invoked.
///
/// This function does not respond.
///
/// \param inode inode of file to close
/// \param handle handle of file to close
/// \param flags file close flags
/// \param user_data user defined context
//------------------------------------------------------------------------------
typedef void wfp_close_fn(
    ino_t inode,
    uint32_t handle,
    int flags,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif
