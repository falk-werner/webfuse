#include "webfuse/provider/impl/url.h"

#include <stdlib.h>
#include <string.h>

struct wfp_impl_url_protocol
{
    char const * name;
    size_t name_length;
    int default_port;
    bool use_tls;
};

static bool wfp_impl_url_readprotocol(
    struct wfp_impl_url * url,
    char const * * data)
{
    static struct wfp_impl_url_protocol const known_protocols[] =
    {
        {"ws://", 5, 80, false},
        {"wss://", 6, 443, true}
    };
    static size_t const count = (sizeof(known_protocols) / sizeof(known_protocols[0]));

    bool found = false;
    for(size_t i = 0; (!found) && (i < count); i++)
    {
        struct wfp_impl_url_protocol const * protocol = &known_protocols[i];
        if (0 == strncmp(*data, protocol->name, protocol->name_length))
        {
            url->port = protocol->default_port;
            url->use_tls = protocol->use_tls;
            *data = *data + protocol->name_length;
            found = true;
        }
    }

    return found;
}

static bool wfp_impl_url_readhost(
    struct wfp_impl_url * url,
    char const * * data)
{
    char * end = strpbrk(*data, ":/");
    bool const result = (NULL != end);

    if (result)
    {
        size_t length = end - *data;
        url->host = strndup(*data, length);
        *data = end;
    }

    return result;
}

static bool wfp_impl_url_readport(
    struct wfp_impl_url * url,
    char const * * data)
{
    bool result;

    if (':' == **data)
    {
        *data = *data + 1;
        char * end = strchr(*data, '/');
        result = (NULL != end);

        if (result)
        {
            url->port = atoi(*data);
            *data = end;
        }
    }
    else
    {
        result = ('/' == **data);
    }

    return result;
}

static bool wfp_impl_url_readpath(
    struct wfp_impl_url * url,
    char const * * data)
{
    bool const result = ('/' == **data);
    url->path = strdup(*data);
    *data = NULL;

    return result;
}


bool wfp_impl_url_init(
    struct wfp_impl_url * url,
    char const * value)
{
    memset(url, 0, sizeof(struct wfp_impl_url));
    char const * data = value;

    bool const result = 
        wfp_impl_url_readprotocol(url, &data) &&
        wfp_impl_url_readhost(url, &data) &&
        wfp_impl_url_readport(url, &data) &&
        wfp_impl_url_readpath(url, &data)
        ;

    if (!result)
    {
        wfp_impl_url_cleanup(url);
    }

    return result;
}

void wfp_impl_url_cleanup(
    struct wfp_impl_url * url)
{    
    free(url->host);
    free(url->path);
    memset(url, 0, sizeof(struct wfp_impl_url));
}

