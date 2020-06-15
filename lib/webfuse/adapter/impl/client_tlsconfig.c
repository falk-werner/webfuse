#include "webfuse/adapter/impl/client_tlsconfig.h"

#include <stdlib.h>
#include <string.h>

void
wf_impl_client_tlsconfig_init(
    struct wf_client_tlsconfig * config)
{
    config->key_path = NULL;
    config->cert_path = NULL;
    config->cafile_path = NULL;
}

void
wf_impl_client_tlsconfig_cleanup(
    struct wf_client_tlsconfig * config)
{
    free(config->key_path);
    free(config->cert_path);
    free(config->cafile_path);
}

void
wf_impl_client_tlsconfig_set_keypath(
    struct wf_client_tlsconfig * config,
    char const * key_path)
{
    free(config->key_path);
    config->key_path = strdup(key_path);
}

void
wf_impl_client_tlsconfig_set_certpath(
    struct wf_client_tlsconfig * config,
    char const * cert_path)
{
    free(config->cert_path);
    config->cert_path = strdup(cert_path);
}

void
wf_impl_client_tlsconfig_set_cafilepath(
    struct wf_client_tlsconfig * config,
    char const * cafile_path)
{
    free(config->cafile_path);
    config->cafile_path = strdup(cafile_path);
}

bool
wf_impl_client_tlsconfig_isset(
    struct wf_client_tlsconfig const * config)
{
    return (NULL != config->cert_path) && (NULL != config->key_path);
}
