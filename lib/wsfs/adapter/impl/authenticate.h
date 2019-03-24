#ifndef WSFS_ADAPTER_IMPL_AUTHENTICATE_H
#define WSFS_ADAPTER_IMPL_AUTHENTICATE_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

struct credentials;

typedef bool authenticate_fn(
    struct credentials * credentials,
    void * user_data);

#ifdef __cplusplus
}
#endif

#endif
