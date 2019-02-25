#ifndef WSFS_ADAPTER_API_H
#define WSFS_ADAPTER_API_H

#ifndef WSFSA_API
#define WSFSA_API
#endif

#ifndef WSFSA_EXPORT
#ifdef __GNUC__
#define WSFSA_EXPORT __attribute__ ((visibility ("default")))
#else
#define WSFSA_EXPORT
#endif
#endif

#endif
