#ifndef WF_ADAPTER_API_H
#define WF_ADAPTER_API_H

#ifndef WF_API
#define WF_API
#endif

#ifndef WF_EXPORT
#ifdef __GNUC__
#define WF_EXPORT __attribute__ ((visibility ("default")))
#else
#define WF_EXPORT
#endif
#endif

#endif
