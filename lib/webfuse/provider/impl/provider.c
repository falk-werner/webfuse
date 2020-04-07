#include "webfuse/provider/impl/provider.h"

#include <stdbool.h>
#include <string.h>

#include "webfuse/provider/impl/request.h"
#include "webfuse/provider/impl/operation/lookup.h"
#include "webfuse/provider/impl/operation/getattr.h"
#include "webfuse/provider/impl/operation/readdir.h"
#include "webfuse/provider/impl/operation/open.h"
#include "webfuse/provider/impl/operation/close.h"
#include "webfuse/provider/impl/operation/read.h"

typedef void wfp_impl_invoke_fn(
    struct wfp_impl_invokation_context * context,
    json_t * params,
    int id);


struct wfp_impl_method
{
    char const * name;
    wfp_impl_invoke_fn * invoke;
    bool is_notification;
};

static void wfp_impl_provider_invoke_method(
    struct wfp_impl_invokation_context * context, 
    char const * method_name,
    json_t * params,
    int id)
{
    static struct wfp_impl_method const methods[] =
    {
        {"lookup", &wfp_impl_lookup, false},
        {"getattr", &wfp_impl_getattr, false},
        {"readdir", &wfp_impl_readdir, false},
        {"open", &wfp_impl_open, false},
        {"close", &wfp_impl_close, true},
        {"read", &wfp_impl_read, false}
    };
    static size_t const count = sizeof(methods) / sizeof(methods[0]);

    for (size_t i = 0; i < count; i++)
    {
        struct wfp_impl_method const * method = &methods[i];
        if (0 == strcmp(method_name, method->name))
        {
            if ((0 < id) || (method->is_notification))
            {
                method->invoke(context, params, id);
            }
            break;
        }
    }
}

void wfp_impl_provider_init(
    struct wfp_provider * provider)
{
    provider->lookup = &wfp_impl_lookup_default;
    provider->getattr = &wfp_impl_getattr_default;
    provider->readdir = &wfp_impl_readdir_default;
    provider->open = &wfp_impl_open_default;
    provider->close = &wfp_impl_close_default;
    provider->read = &wfp_impl_read_default;
    provider->connected = &wfp_impl_connected_default;
    provider->disconnected = &wfp_impl_disconnected_default;
    provider->get_credentials = NULL;
}

void wfp_impl_provider_init_from_prototype(
    struct wfp_provider * provider,
    struct wfp_provider const * prototype)
{
    provider->lookup =  prototype->lookup;
    provider->getattr = prototype->getattr;
    provider->readdir =  prototype->readdir;
    provider->open =  prototype->open;
    provider->close =  prototype->close;
    provider->read = prototype->read;
    provider->connected =  prototype->connected;
    provider->disconnected = prototype->disconnected;
    provider->get_credentials = prototype->get_credentials;
}

void wfp_impl_provider_invoke(
    struct wfp_impl_invokation_context * context,
    json_t * request)
{
    json_t * method_holder = json_object_get(request, "method");
    json_t * params = json_object_get(request, "params");
    json_t * id_holder = json_object_get(request, "id");

    if ((json_is_string(method_holder)) && (json_is_array(params)))
    {
        char const * method = json_string_value(method_holder);
        int id = json_is_integer(id_holder) ? json_integer_value(id_holder) : 0;

        wfp_impl_provider_invoke_method(context, method, params, id);
    }
}

void wfp_impl_connected_default(
    void * user_data)
{
    (void) user_data;

    // empty
}

void wfp_impl_disconnected_default(
    void * user_data)
{
    (void) user_data;

    // empty
}

 bool wfp_impl_provider_is_authentication_enabled(
    struct wfp_provider * provider)
{
    return (NULL != provider->get_credentials);
}

