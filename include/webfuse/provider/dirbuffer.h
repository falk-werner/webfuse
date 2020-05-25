////////////////////////////////////////////////////////////////////////////////
/// \file provider/dirbuffer.h
/// \brief Buffer used for directory listing.
////////////////////////////////////////////////////////////////////////////////

#ifndef WF_PROVIDER_DIRBUFFER_H
#define WF_PROVIDER_DIRBUFFER_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "webfuse/provider/api.h"

#ifdef __cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// \struct wfp_dirbuffer
/// \brief Buffer used for directory listing.
///
/// \see wfp_respond_readdir
//------------------------------------------------------------------------------
struct wfp_dirbuffer;

//------------------------------------------------------------------------------
/// \brief Creates a new dir buffer.
///
/// \return newly created dir buffer.
//------------------------------------------------------------------------------
extern WFP_API struct wfp_dirbuffer * wfp_dirbuffer_create(void);

//------------------------------------------------------------------------------
/// \brief Disposes a dir buffer.
///
/// \param buffer pointer to dir buffer
//------------------------------------------------------------------------------
extern WFP_API void wfp_dirbuffer_dispose(
    struct wfp_dirbuffer * buffer);

//------------------------------------------------------------------------------
/// \brief Adds an entry to dir buffer.
///
/// \param buffer pointer to dir buffer
/// \param name name of the entry (file or directory)
/// \param inode inode of the entry
//------------------------------------------------------------------------------
extern WFP_API void wfp_dirbuffer_add(
    struct wfp_dirbuffer * buffer,
    char const * name,
    ino_t inode);

#ifdef __cplusplus
}
#endif


#endif
