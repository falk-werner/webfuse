#include "webfuse/impl/jsonrpc/proxy_request_manager.h"
#include "webfuse/status.h"
#include "webfuse/impl/timer/timer.h"
#include "webfuse/impl/jsonrpc/response_intern.h"
#include "webfuse/impl/jsonrpc/error.h"

#include <stdlib.h>
#include <limits.h>

struct wf_timer;

struct wf_jsonrpc_proxy_request
{
    struct wf_jsonrpc_proxy_request_manager * manager;
    int id;
    wf_jsonrpc_proxy_finished_fn * finished;
    void * user_data;
    struct wf_timer * timer;
    struct wf_jsonrpc_proxy_request * next;
};

struct wf_jsonrpc_proxy_request_manager
{
    struct wf_timer_manager * timer_manager;
    int timeout;
    int id;
    struct wf_jsonrpc_proxy_request * requests;
};

static void
wf_impl_jsonrpc_proxy_request_on_timeout(
    struct wf_timer * timer,
    void * user_data)
{
    struct wf_jsonrpc_proxy_request * request = user_data;

    wf_impl_jsonrpc_proxy_request_manager_cancel_request(
        request->manager,
        request->id,
        WF_BAD_TIMEOUT,
        "Timeout");
}

static int
wf_impl_jsonrpc_proxy_request_manager_next_id(
    struct wf_jsonrpc_proxy_request_manager * manager)
{
    if (manager->id < INT_MAX)
    {
        manager->id++;
    }
    else
    {
        manager->id = 1;
    }
    
    return manager->id;
}


struct wf_jsonrpc_proxy_request_manager *
wf_impl_jsonrpc_proxy_request_manager_create(
    struct wf_timer_manager * timer_manager,
    int timeout)
{
    struct wf_jsonrpc_proxy_request_manager * manager = malloc(sizeof(struct wf_jsonrpc_proxy_request_manager));
    manager->id = 1;
    manager->timer_manager = timer_manager;
    manager->timeout = timeout;
    manager->requests = NULL;

    return manager;
}

void
wf_impl_jsonrpc_proxy_request_manager_dispose(
    struct wf_jsonrpc_proxy_request_manager * manager)
{
    struct wf_jsonrpc_proxy_request * request = manager->requests;
    while (NULL != request)
    {
        struct wf_jsonrpc_proxy_request * next = request->next;

        wf_impl_jsonrpc_propate_error(
            request->finished, request->user_data,
            WF_BAD, "Bad: cancelled pending request during shutdown");
        wf_impl_timer_cancel(request->timer);
        wf_impl_timer_dispose(request->timer);
        free(request);
        request = next;
    }
    
    free(manager);
}

int
wf_impl_jsonrpc_proxy_request_manager_add_request(
    struct wf_jsonrpc_proxy_request_manager * manager,
    wf_jsonrpc_proxy_finished_fn * finished,
    void * user_data)
{
    struct wf_jsonrpc_proxy_request * request = malloc(sizeof(struct wf_jsonrpc_proxy_request));
    request->finished = finished;
    request->user_data = user_data;
    request->manager = manager;
    request->id = wf_impl_jsonrpc_proxy_request_manager_next_id(manager);
    request->timer = wf_impl_timer_create(manager->timer_manager,
        &wf_impl_jsonrpc_proxy_request_on_timeout ,request);
    wf_impl_timer_start(request->timer, manager->timeout);

    request->next = manager->requests;
    manager->requests = request;

    return request->id;
}

void
wf_impl_jsonrpc_proxy_request_manager_cancel_request(
    struct wf_jsonrpc_proxy_request_manager * manager,
    int id,
    int error_code,
    char const * error_message)
{
    struct wf_jsonrpc_proxy_request * prev = NULL;
    struct wf_jsonrpc_proxy_request * request = manager->requests;
    while (request != NULL)
    {
        struct wf_jsonrpc_proxy_request * next = request->next;
        if (id == request->id)
        {
            wf_impl_jsonrpc_propate_error(
                request->finished, request->user_data,
                error_code, error_message);
            wf_impl_timer_cancel(request->timer);
            wf_impl_timer_dispose(request->timer);
            free(request);
            
            if (NULL != prev)
            {
                prev->next = next;
            }
            else
            {
                manager->requests = next;
            }
            break;
        }

        prev = request;
        request = next;
    }
}

void
wf_impl_jsonrpc_proxy_request_manager_finish_request(
    struct wf_jsonrpc_proxy_request_manager * manager,
    struct wf_jsonrpc_response * response)
{
    struct wf_jsonrpc_proxy_request * prev = NULL;
    struct wf_jsonrpc_proxy_request * request = manager->requests;
    while (request != NULL)
    {
        struct wf_jsonrpc_proxy_request * next = request->next;
        if (response->id == request->id)
        {
            wf_jsonrpc_proxy_finished_fn * finished = request->finished;
            void * user_data = request->user_data;

            wf_impl_timer_cancel(request->timer);
            wf_impl_timer_dispose(request->timer);
            free(request);
            
            if (NULL != prev)
            {
                prev->next = next;
            }
            else
            {
                manager->requests = next;
            }

            finished(user_data, response->result, response->error);
            break;
        }

        prev = request;
        request = next;
    }
}
