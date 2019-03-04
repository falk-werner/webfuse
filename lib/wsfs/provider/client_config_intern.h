#ifndef WSFS_PROVIDER_CLIENT_CONFIG_INTERN_H
#define WSFS_PROVIDER_CLIENT_CONFIG_INTERN_H

#include "wsfs/provider/client_config.h"
#include "wsfs/provider/provider.h"

struct wsfsp_client_config
{
    struct wsfsp_provider provider;
    void * user_data;
    char * key_path;
    char * cert_path;
};

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

#endif
