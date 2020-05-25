////////////////////////////////////////////////////////////////////////////////
/// \file adapter/mountpoint_factory.h
/// \brief Defines a factory function to create mointpoints.
////////////////////////////////////////////////////////////////////////////////

#ifndef WF_ADAPTER_MOUNTPOINT_FACTORY_H
#define WF_ADAPTER_MOUNTPOINT_FACTORY_H

#include <webfuse/adapter/api.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct wf_mountpoint;

//------------------------------------------------------------------------------
/// \brief Factory function to create mountpoints.
///
/// \param filesystem name the filesystem
/// \param user_data context of the factory
/// \return newly created mountpoint or NULL, on error
///
/// \see wf_server_config_set_mountpoint_factory
/// \see wf_server_protocol_create
//------------------------------------------------------------------------------
typedef struct wf_mountpoint *
wf_create_mountpoint_fn(
    char const * filesystem,
    void * user_data);


#ifdef __cplusplus
}
#endif


#endif
