#ifndef _WSFS_API_H
#define _WSFS_API_H

#ifndef WSFS_API
#define WSFS_API
#endif

#ifndef WSFS_EXPORT
#ifdef __GNUC__
#define WSFS_EXPORT __attribute__ ((visibility ("default")))
#else
#define WSFS_EXPORT
#endif
#endif

#endif
