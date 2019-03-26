#ifndef WF_PROVIDER_IMPL_URL_H
#define WF_PROVIDER_IMPL_URL_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif
struct wfp_impl_url
{
    char * host;
    int port;
    char * path;
    bool use_tls;
};

extern bool wfp_impl_url_init(
    struct wfp_impl_url * url,
    char const * value);

extern void wfp_impl_url_cleanup(
    struct wfp_impl_url * url);


#ifdef __cplusplus
}
#endif

#endif
