#ifndef WSFS_UTIL_H
#define WSFS_UTIL_H

#ifdef __GNUC__
#define WSFS_UNUSED_PARAM(param) param __attribute__((unused))
#else
#define WSFS_UNUSED_PARAM(param)
#endif

#endif
