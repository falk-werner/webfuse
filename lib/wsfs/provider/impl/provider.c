#include "wsfs/provider/impl/provider.h"

#include <stdbool.h>
#include <string.h>

#include "wsfs/provider/impl/request.h"
#include "wsfs/provider/impl/operation/lookup.h"
#include "wsfs/provider/impl/operation/getattr.h"
#include "wsfs/provider/impl/operation/readdir.h"
#include "wsfs/provider/impl/operation/open.h"
#include "wsfs/provider/impl/operation/close.h"
#include "wsfs/provider/impl/operation/read.h"

typedef void wsfsp_impl_invoke_fn(
    struct wsfsp_impl_invokation_context * context,
    json_t * params,
    int id);


struct wsfsp_impl_method
{
    char const * name;
    wsfsp_impl_invoke_fn * invoke;
    bool is_notification;
};

static void wsfsp_impl_provider_invoke_method(
    struct wsfsp_impl_invokation_context * context, 
    char const * method_name,
    json_t * params,
    int id)
{
    static struct wsfsp_impl_method const methods[] =
    {
        {"lookup", &wsfsp_impl_lookup, false},
        {"getattr", &wsfsp_impl_getattr, false},
        {"readdir", &wsfsp_impl_readdir, false},
        {"open", &wsfsp_impl_open, false},
        {"close", &wsfsp_impl_close, true},
        {"read", &wsfsp_impl_read, false}
    };
    static size_t const count = sizeof(methods) / sizeof(methods[0]);

    for (size_t i = 0; i < count; i++)
    {
        struct wsfsp_impl_method const * method = &methods[i];
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

void wsfsp_impl_provider_init(
    struct wsfsp_provider * provider)
{
    provider->lookup = &wsfsp_impl_lookup_default;
    provider->getattr = &wsfsp_impl_getattr_default;
    provider->readdir = &wsfsp_impl_readdir_default;
    provider->open = &wsfsp_impl_open_default;
    provider->close = &wsfsp_impl_close_default;
    provider->read = &wsfsp_impl_read_default;
    provider->connected = &wsfsp_impl_connected_default;
    provider->disconnected = &wsfsp_impl_disconnected_default;
    provider->ontimer = &wsfsp_impl_ontimer_default;
}

void wsfsp_impl_provider_init_from_prototype(
    struct wsfsp_provider * provider,
    struct wsfsp_provider const * prototype)
{
    provider->lookup =  prototype->lookup;
    provider->getattr = prototype->getattr;
    provider->readdir =  prototype->readdir;
    provider->open =  prototype->open;
    provider->close =  prototype->close;
    provider->read = prototype->read;
    provider->connected =  prototype->connected;
    provider->disconnected = prototype->disconnected;
    provider->ontimer = prototype->ontimer;
}

void wsfsp_impl_provider_invoke(
    struct wsfsp_impl_invokation_context * context,
    json_t * request)
{
    json_t * method_holder = json_object_get(request, "method");
    json_t * params = json_object_get(request, "params");
    json_t * id_holder = json_object_get(request, "id");

    if ((NULL != method_holder) && (json_is_string(method_holder)) &&
        (NULL != params) && (json_is_array(params)))
    {
        char const * method = json_string_value(method_holder);
        int id = json_is_integer(id_holder) ? json_integer_value(id_holder) : 0;

        wsfsp_impl_provider_invoke_method(context, method, params, id);
    }
}

void wsfsp_impl_connected_default(
    void * user_data)
{
    (void) user_data;

    // empty
}

void wsfsp_impl_disconnected_default(
    void * user_data)
{
    (void) user_data;

    // empty
}

void wsfsp_impl_ontimer_default(
    void * user_data)
{
    (void) user_data;

    // empty
}