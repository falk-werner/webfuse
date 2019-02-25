#ifndef WSFSP_URL_H
#define WSFSP_URL_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

struct wsfsp_url
{
    char * host;
    int port;
    char * path;
    bool use_tls;
};

#ifdef __cplusplus
extern "C"
{
#endif

extern bool wsfsp_url_init(
    struct wsfsp_url * url,
    char const * value);

extern void wsfsp_url_cleanup(
    struct wsfsp_url * url);


#ifdef __cplusplus
}
#endif

#endif
