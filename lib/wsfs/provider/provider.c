#include "wsfs/provider/provider_intern.h"

#include <stdbool.h>
#include <string.h>

#include "wsfs/provider/request.h"
#include "wsfs/provider/operation/lookup_intern.h"
#include "wsfs/provider/operation/getattr_intern.h"
#include "wsfs/provider/operation/readdir_intern.h"
#include "wsfs/provider/operation/open_intern.h"
#include "wsfs/provider/operation/close_intern.h"
#include "wsfs/provider/operation/read_intern.h"

typedef void wsfsp_invoke_fn(
    struct wsfsp_invokation_context * context,
    json_t * params,
    int id);


struct wsfsp_method
{
    char const * name;
    wsfsp_invoke_fn * invoke;
    bool is_notification;
};

static void wsfsp_provider_invoke_method(
    struct wsfsp_invokation_context * context, 
    char const * method_name,
    json_t * params,
    int id)
{
    static struct wsfsp_method const methods[] =
    {
        {"lookup", &wsfsp_lookup, false},
        {"getattr", &wsfsp_getattr, false},
        {"readdir", &wsfsp_readdir, false},
        {"open", &wsfsp_open, false},
        {"close", &wsfsp_close, true},
        {"read", &wsfsp_read, false}
    };
    static size_t const count = sizeof(methods) / sizeof(methods[0]);

    for (size_t i = 0; i < count; i++)
    {
        struct wsfsp_method const * method = &methods[i];
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

void wsfsp_provider_invoke(
    struct wsfsp_invokation_context * context,
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

        wsfsp_provider_invoke_method(context, method, params, id);
    }
}

void wsfsp_connected_default(
    void * user_data)
{
    (void) user_data;

    // empty
}

void wsfsp_disconnected_default(
    void * user_data)
{
    (void) user_data;

    // empty
}

void wsfsp_ontimer_default(
    void * user_data)
{
    (void) user_data;

    // empty
}