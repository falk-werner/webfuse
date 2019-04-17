#ifndef WF_CORE_STRING_H
#define WF_CORE_STRING_H

#ifndef __cplusplus
#include <stdarg.h>
#else
#include <cstdarg>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

extern char * wf_create_string(char const * format, ...);

#ifdef __cplusplus
}
#endif

#endif
