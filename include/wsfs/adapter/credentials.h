#ifndef WSFS_ADAPTER_CREDENTIALS_H
#define WSFS_ADAPTER_CREDENTIALS_H

#include "wsfs/adapter/api.h"

#ifdef __cplusplus
extern "C"
{
#endif

struct wsfs_credentials;

extern WSFSA_API char const * wsfs_credentials_type(
    struct wsfs_credentials const * credentials);

extern WSFSA_API char const * wsfs_credentials_get(
    struct wsfs_credentials const * credentials,
    char const * key);

#ifdef __cplusplus
}
#endif


#endif
