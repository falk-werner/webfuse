#ifndef WF_IMPL_UTIL_URL_H
#define WF_IMPL_UTIL_URL_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif
struct wf_url
{
    char * host;
    int port;
    char * path;
    bool use_tls;
};

extern bool wf_impl_url_init(
    struct wf_url * url,
    char const * value);

extern void wf_impl_url_cleanup(
    struct wf_url * url);


#ifdef __cplusplus
}
#endif

#endif
