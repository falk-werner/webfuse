#ifndef _WSFSP_API_H
#define _WSFSP_API_H

#ifndef WSFSP_API
#define WSFSP_API
#endif

#ifndef WSFSP_EXPORT
#ifdef __GNUC__
#define WSFSP_EXPORT __attribute__ ((visibility ("default")))
#else
#define WSFSP_EXPORT
#endif
#endif

#endif
