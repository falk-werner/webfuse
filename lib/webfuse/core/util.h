#ifndef WF_UTIL_H
#define WF_UTIL_H

#ifdef __GNUC__
#define WF_UNUSED_PARAM(param) param __attribute__((unused))
#else
#define WF_UNUSED_PARAM(param)
#endif

#endif
