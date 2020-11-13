////////////////////////////////////////////////////////////////////////////////
/// \file webfuse/mountpoint.h
/// \brief Mointpoint.
////////////////////////////////////////////////////////////////////////////////

#ifndef WF_MOUNTPOINT_H
#define WF_MOUNTPOINT_H

#include <webfuse/api.h>

#ifdef __cplusplus
extern "C"
{
#endif

//------------------------------------------------------------------------------
/// \struct wf_mountpoint
/// \brief Mointpoint.
//------------------------------------------------------------------------------
struct wf_mountpoint;

//------------------------------------------------------------------------------
/// \brief Disposes the user defined context of a mountpoint.
///
/// \param user_data user defined context of the mointpoint.
///
/// \see wf_mountpoint_set_userdata
//------------------------------------------------------------------------------
typedef void
wf_mountpoint_userdata_dispose_fn(
    void * user_data);

//------------------------------------------------------------------------------
/// \brief Creates a mountpoint.
///
/// \param path local path of the mounpoint
/// \return Newly created mountpoint.
//------------------------------------------------------------------------------
extern WF_API struct wf_mountpoint *
wf_mountpoint_create(
    char const * path);

//------------------------------------------------------------------------------
/// \brief Disposes a mountpoint.
///
/// \param mountpoint pointer to the mountpoint
//------------------------------------------------------------------------------
extern WF_API void
wf_mountpoint_dispose(
    struct wf_mountpoint * mountpoint);

//------------------------------------------------------------------------------
/// \brief Returns the local path of the mountpoint.
///
/// \param mountpoint pointer to the mountpoint
/// \return local path of the mountpoint
//------------------------------------------------------------------------------
extern WF_API char const *
wf_mountpoint_get_path(
    struct wf_mountpoint const * mountpoint);

//------------------------------------------------------------------------------
/// \brief Sets user data of the mointpoint.
///
/// \note This function is intended for custom mountpoint factories to
///       annotate mountpoints with a user specified context.
///
/// \param mounpoint pointer to the mountpoint
/// \param user_data user data
/// \param dispose pointer to dipose function of user data or NULL,
///                if there is no need to dispose user data
//------------------------------------------------------------------------------
extern WF_API void
wf_mountpoint_set_userdata(
    struct wf_mountpoint * mointpoint,
    void * user_data,
    wf_mountpoint_userdata_dispose_fn * dispose);

//------------------------------------------------------------------------------
/// \brief Adds a mount option.
///
/// Mount options are passed to libfuse when a filesystem is mounted.
/// See libfuse documenation for allowed values.
///
/// \param mountpoint pointer to the mountpooint
/// \param option value of the mount option
//------------------------------------------------------------------------------
extern WF_API void
wf_mountpoint_add_mountoption(
    struct wf_mountpoint * mountpoint,
    char const * option);

#ifdef __cplusplus
}
#endif


#endif
