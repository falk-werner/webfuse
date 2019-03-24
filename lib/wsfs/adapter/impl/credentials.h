#ifndef WSFS_ADAPTER_IMPL_CREDENTIALS_H
#define WSFS_ADAPTER_IMPL_CREDENTIALS_H

#include <jansson.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct credentials
{
    char * type;
    json_t * data;
};

extern void credentials_init(
    struct credentials * credentials,
    char const * type,
    json_t * data);

extern void credentials_cleanup(
    struct credentials * credentials);

extern char const * credentials_type(
    struct credentials const * credentials);

extern char const * credentials_get(
    struct credentials const * credentials,
    char const * key);

#ifdef __cplusplus
}
#endif

#endif