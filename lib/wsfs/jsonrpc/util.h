#ifndef _WSFS_JSON_UTIL_H
#define _WSFS_JSON_UTIL_H

#include <jansson.h>

#ifdef __cplusplus
extern "C"
{
#endif

extern int wsfs_json_get_int(json_t const * object, char const * key, int default_value);

#ifdef __cplusplus
}
#endif

#endif
