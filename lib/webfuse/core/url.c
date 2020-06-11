#include "webfuse/core/url.h"

#include <stdlib.h>
#include <string.h>

struct wf_url_protocol
{
    char const * name;
    size_t name_length;
    int default_port;
    bool use_tls;
};

static bool wf_url_readprotocol(
    struct wf_url * url,
    char const * * data)
{
    static struct wf_url_protocol const known_protocols[] =
    {
        {"ws://", 5, 80, false},
        {"wss://", 6, 443, true}
    };
    static size_t const count = (sizeof(known_protocols) / sizeof(known_protocols[0]));

    bool found = false;
    for(size_t i = 0; (!found) && (i < count); i++)
    {
        struct wf_url_protocol const * protocol = &known_protocols[i];
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

static bool wf_url_readhost(
    struct wf_url * url,
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

static bool wf_url_readport(
    struct wf_url * url,
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

static bool wf_url_readpath(
    struct wf_url * url,
    char const * * data)
{
    bool const result = ('/' == **data);
    url->path = strdup(*data);
    *data = NULL;

    return result;
}


bool wf_url_init(
    struct wf_url * url,
    char const * value)
{
    memset(url, 0, sizeof(struct wf_url));
    char const * data = value;

    bool const result = 
        wf_url_readprotocol(url, &data) &&
        wf_url_readhost(url, &data) &&
        wf_url_readport(url, &data) &&
        wf_url_readpath(url, &data)
        ;

    if (!result)
    {
        wf_url_cleanup(url);
    }

    return result;
}

void wf_url_cleanup(
    struct wf_url * url)
{    
    free(url->host);
    free(url->path);
    memset(url, 0, sizeof(struct wf_url));
}

