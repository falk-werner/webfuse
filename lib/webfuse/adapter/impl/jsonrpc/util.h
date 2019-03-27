#ifndef WF_ADAPTER_IMPL_JSON_UTIL_H
#define WF_ADAPTER_IMPL_JSON_UTIL_H

#include <jansson.h>

#ifdef __cplusplus
extern "C"
{
#endif

extern int wf_impl_json_get_int(json_t const * object, char const * key, int default_value);

#ifdef __cplusplus
}
#endif

#endif
