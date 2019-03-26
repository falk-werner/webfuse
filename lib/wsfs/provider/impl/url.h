#ifndef WSFS_PROVIDER_IMPL_URL_H
#define WSFS_PROVIDER_IMPL_URL_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif
struct wsfsp_impl_url
{
    char * host;
    int port;
    char * path;
    bool use_tls;
};

extern bool wsfsp_impl_url_init(
    struct wsfsp_impl_url * url,
    char const * value);

extern void wsfsp_impl_url_cleanup(
    struct wsfsp_impl_url * url);


#ifdef __cplusplus
}
#endif

#endif
