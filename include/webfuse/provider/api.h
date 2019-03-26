#ifndef WFP_PROVIDER_API_H
#define WFP_PROVIDER_API_H

#ifndef WFP_API
#define WFP_API
#endif

#ifndef WFP_EXPORT
#ifdef __GNUC__
#define WFP_EXPORT __attribute__ ((visibility ("default")))
#else
#define WFP_EXPORT
#endif
#endif

#endif
