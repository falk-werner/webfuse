#ifndef WSFS_PROVIDER_DIRBUFFER_INTERN_H
#define WSFS_PROVIDER_DIRBUFFER_INTERN_H

#include "wsfs/provider/dirbuffer.h"
#include <jansson.h>

struct wsfsp_dirbuffer
{
    json_t * entries;
};

extern json_t * wsfsp_dirbuffer_take(
    struct wsfsp_dirbuffer * buffer);

#endif
