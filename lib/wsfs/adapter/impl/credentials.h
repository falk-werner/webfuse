#ifndef WSFS_ADAPTER_IMPL_CREDENTIALS_H
#define WSFS_ADAPTER_IMPL_CREDENTIALS_H

#include <jansson.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct wsfs_credentials
{
    char * type;
    json_t * data;
};

extern void wsfs_impl_credentials_init(
    struct wsfs_credentials * credentials,
    char const * type,
    json_t * data);

extern void wsfs_impl_credentials_cleanup(
    struct wsfs_credentials * credentials);

extern char const * wsfs_impl_credentials_type(
    struct wsfs_credentials const * credentials);

extern char const * wsfs_impl_credentials_get(
    struct wsfs_credentials const * credentials,
    char const * key);

#ifdef __cplusplus
}
#endif

#endif