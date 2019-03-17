#ifndef WSFS_ADAPTER_CREDENTIALS_INTERN_H
#define WSFS_ADAPTER_CREDENTIALS_INTERN_H

#include "wsfs/adapter/credentials.h"
#include <jansson.h>

struct wsfs_credentials
{
    char * type;
    json_t * data;
};

#ifdef __cplusplus
extern "C"
{
#endif

extern void wsfs_credentials_init(
    struct wsfs_credentials * credentials,
    char const * type,
    json_t * data);

extern void wsfs_credentials_cleanup(
    struct wsfs_credentials * credentials);

#ifdef __cplusplus
}
#endif

#endif