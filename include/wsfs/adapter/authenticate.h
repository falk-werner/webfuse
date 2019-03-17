#ifndef WSFS_ADAPTER_AUTHENTICATE_H
#define WSFS_ADAPTER_AUTHENTICATE_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

struct wsfs_credentials;

typedef bool wsfs_authenticate_fn(
    struct wsfs_credentials * credentials,
    void * user_data);


#endif
