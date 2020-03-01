#include <gtest/gtest.h>
#include "wf/jsonrpc/proxy.h"
#include "wf/timer/manager.h"
#include "webfuse/core/json_util.h"

#include <thread>
#include <chrono>

using namespace std::chrono_literals;


#define WF_DEFAULT_TIMEOUT (10 * 1000)

namespace
{

    struct SendContext
    {
        json_t * response;
        bool result;
        bool is_called;

        explicit SendContext(bool result_ = true)
        : response(nullptr)
        , result(result_)
        , is_called(false)
        {
        }

        ~SendContext() 
        {
            if (nullptr != response)
            {
                json_decref(response);
            }
        }
    };

    bool jsonrpc_send(
        json_t * request,
        void * user_data)
    {
        SendContext * context = reinterpret_cast<SendContext*>(user_data);
        context->is_called = true;
        context->response = request;
        json_incref(request);

        return context->result;
    }

    struct FinishedContext
    {
        bool is_called;
        json_t * result;
        json_t * error;

        FinishedContext()
        : is_called(false)
        , result(nullptr)
        , error(nullptr)
        {

        }

        ~FinishedContext()
        {
            if (nullptr != result)
            {
                json_decref(result);
            }

            if (nullptr != error)
            {
                json_decref(error);
            }
        }
    };

    void jsonrpc_finished(
        void * user_data,
        json_t const * result,
        json_t const * error)
    {
        FinishedContext * context = reinterpret_cast<FinishedContext*>(user_data);
        context->is_called = true;
        context->result = json_deep_copy(result);
        context->error = json_deep_copy(error);
    }
}

TEST(wf_jsonrpc_proxy, init)
{
    struct wf_timer_manager * timer_manager = wf_timer_manager_create();

    SendContext context;
    void * user_data = reinterpret_cast<void*>(&context);
    struct wf_jsonrpc_proxy * proxy = wf_jsonrpc_proxy_create(timer_manager, WF_DEFAULT_TIMEOUT, &jsonrpc_send, user_data);

    wf_jsonrpc_proxy_dispose(proxy);
    wf_timer_manager_dispose(timer_manager);

    ASSERT_FALSE(context.is_called);
}

TEST(wf_jsonrpc_proxy, invoke)
{
    struct wf_timer_manager * timer_manager = wf_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wf_jsonrpc_proxy * proxy = wf_jsonrpc_proxy_create(timer_manager, WF_DEFAULT_TIMEOUT, &jsonrpc_send, send_data);

    FinishedContext finished_context;
    void * finished_data = reinterpret_cast<void*>(&finished_context);
    wf_jsonrpc_proxy_invoke(proxy, &jsonrpc_finished, finished_data, "foo", "si", "bar", 42);

    ASSERT_TRUE(send_context.is_called);
    ASSERT_TRUE(json_is_object(send_context.response));

    json_t * method = json_object_get(send_context.response, "method");
    ASSERT_TRUE(json_is_string(method));
    ASSERT_STREQ("foo", json_string_value(method));

    json_t * params = json_object_get(send_context.response, "params");
    ASSERT_TRUE(json_is_array(params));
    ASSERT_EQ(2, json_array_size(params));
    ASSERT_TRUE(json_is_string(json_array_get(params, 0)));
    ASSERT_STREQ("bar", json_string_value(json_array_get(params, 0)));
    ASSERT_TRUE(json_is_integer(json_array_get(params, 1)));
    ASSERT_EQ(42, json_integer_value(json_array_get(params, 1)));

    json_t * id = json_object_get(send_context.response, "id");
    ASSERT_TRUE(json_is_integer(id));

    ASSERT_FALSE(finished_context.is_called);

    wf_jsonrpc_proxy_dispose(proxy);
    wf_timer_manager_dispose(timer_manager);

    ASSERT_TRUE(finished_context.is_called);
    ASSERT_FALSE(nullptr == finished_context.error);
}

TEST(wf_jsonrpc_proxy, invoke_calls_finish_if_send_fails)
{
    struct wf_timer_manager * timer_manager = wf_timer_manager_create();

    SendContext send_context(false);
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wf_jsonrpc_proxy * proxy = wf_jsonrpc_proxy_create(timer_manager, WF_DEFAULT_TIMEOUT, &jsonrpc_send, send_data);

    FinishedContext finished_context;
    void * finished_data = reinterpret_cast<void*>(&finished_context);
    wf_jsonrpc_proxy_invoke(proxy, &jsonrpc_finished, finished_data, "foo", "si", "bar", 42);

    ASSERT_TRUE(send_context.is_called);
    ASSERT_TRUE(json_is_object(send_context.response));

    ASSERT_TRUE(finished_context.is_called);
    ASSERT_FALSE(nullptr == finished_context.error);

    wf_jsonrpc_proxy_dispose(proxy);
    wf_timer_manager_dispose(timer_manager);
}

TEST(wf_jsonrpc_proxy, invoke_fails_if_another_request_is_pending)
{
    struct wf_timer_manager * timer_manager = wf_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wf_jsonrpc_proxy * proxy = wf_jsonrpc_proxy_create(timer_manager, WF_DEFAULT_TIMEOUT, &jsonrpc_send, send_data);

    FinishedContext finished_context;
    void * finished_data = reinterpret_cast<void*>(&finished_context);
    wf_jsonrpc_proxy_invoke(proxy, &jsonrpc_finished, finished_data, "foo", "si", "bar", 42);

    FinishedContext finished_context2;
    void * finished_data2 = reinterpret_cast<void*>(&finished_context2);
    wf_jsonrpc_proxy_invoke(proxy, &jsonrpc_finished, finished_data2, "foo", "");

    ASSERT_TRUE(send_context.is_called);
    ASSERT_TRUE(json_is_object(send_context.response));

    ASSERT_FALSE(finished_context.is_called);

    ASSERT_TRUE(finished_context2.is_called);
    ASSERT_EQ(WF_BAD_BUSY, wf_impl_jsonrpc_get_status(finished_context2.error));

    wf_jsonrpc_proxy_dispose(proxy);
    wf_timer_manager_dispose(timer_manager);
}

TEST(wf_jsonrpc_proxy, invoke_fails_if_request_is_invalid)
{
    struct wf_timer_manager * timer_manager = wf_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wf_jsonrpc_proxy * proxy = wf_jsonrpc_proxy_create(timer_manager, WF_DEFAULT_TIMEOUT, &jsonrpc_send, send_data);

    FinishedContext finished_context;
    void * finished_data = reinterpret_cast<void*>(&finished_context);
    wf_jsonrpc_proxy_invoke(proxy, &jsonrpc_finished, finished_data, "foo", "?", "error");

    ASSERT_FALSE(send_context.is_called);

    ASSERT_TRUE(finished_context.is_called);
    ASSERT_EQ(WF_BAD, wf_impl_jsonrpc_get_status(finished_context.error));

    wf_jsonrpc_proxy_dispose(proxy);
    wf_timer_manager_dispose(timer_manager);
}

TEST(wf_jsonrpc_proxy, on_result)
{
    struct wf_timer_manager * timer_manager = wf_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wf_jsonrpc_proxy * proxy = wf_jsonrpc_proxy_create(timer_manager, WF_DEFAULT_TIMEOUT, &jsonrpc_send, send_data);

    FinishedContext finished_context;
    void * finished_data = reinterpret_cast<void*>(&finished_context);
    wf_jsonrpc_proxy_invoke(proxy, &jsonrpc_finished, finished_data, "foo", "si", "bar", 42);

    ASSERT_TRUE(send_context.is_called);
    ASSERT_TRUE(json_is_object(send_context.response));

    json_t * id = json_object_get(send_context.response, "id");
    ASSERT_TRUE(json_is_number(id));

    json_t * response = json_object();
    json_object_set_new(response, "result", json_string("okay"));
    json_object_set(response, "id", id);

    wf_jsonrpc_proxy_onresult(proxy, response);
    json_decref(response);

    ASSERT_TRUE(finished_context.is_called);
    ASSERT_EQ(nullptr, finished_context.error);
    ASSERT_TRUE(json_is_string(finished_context.result));
    ASSERT_STREQ("okay", json_string_value(finished_context.result));

    wf_jsonrpc_proxy_dispose(proxy);
    wf_timer_manager_dispose(timer_manager);
}

TEST(wf_jsonrpc_proxy, on_result_reject_response_with_unknown_id)
{
    struct wf_timer_manager * timer_manager = wf_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wf_jsonrpc_proxy * proxy = wf_jsonrpc_proxy_create(timer_manager, WF_DEFAULT_TIMEOUT, &jsonrpc_send, send_data);

    FinishedContext finished_context;
    void * finished_data = reinterpret_cast<void*>(&finished_context);
    wf_jsonrpc_proxy_invoke(proxy, &jsonrpc_finished, finished_data, "foo", "si", "bar", 42);

    ASSERT_TRUE(send_context.is_called);
    ASSERT_TRUE(json_is_object(send_context.response));

    json_t * id = json_object_get(send_context.response, "id");
    ASSERT_TRUE(json_is_number(id));

    json_t * response = json_object();
    json_object_set_new(response, "result", json_string("okay"));
    json_object_set_new(response, "id", json_integer(1 + json_integer_value(id)));

    wf_jsonrpc_proxy_onresult(proxy, response);
    json_decref(response);

    ASSERT_FALSE(finished_context.is_called);

    wf_jsonrpc_proxy_dispose(proxy);
    wf_timer_manager_dispose(timer_manager);
}

TEST(wf_jsonrpc_proxy, timeout)
{
    struct wf_timer_manager * timer_manager = wf_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wf_jsonrpc_proxy * proxy = wf_jsonrpc_proxy_create(timer_manager, 0, &jsonrpc_send, send_data);

    FinishedContext finished_context;
    void * finished_data = reinterpret_cast<void*>(&finished_context);
    wf_jsonrpc_proxy_invoke(proxy, &jsonrpc_finished, finished_data, "foo", "si", "bar", 42);

    ASSERT_TRUE(send_context.is_called);
    ASSERT_TRUE(json_is_object(send_context.response));

    std::this_thread::sleep_for(10ms);
    wf_timer_manager_check(timer_manager);

    ASSERT_TRUE(finished_context.is_called);
    ASSERT_EQ(WF_BAD_TIMEOUT, wf_impl_jsonrpc_get_status(finished_context.error));

    wf_jsonrpc_proxy_dispose(proxy);
    wf_timer_manager_dispose(timer_manager);
}

TEST(wf_jsonrpc_proxy, cleanup_pending_request)
{
    struct wf_timer_manager * timer_manager = wf_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wf_jsonrpc_proxy * proxy = wf_jsonrpc_proxy_create(timer_manager, 10, &jsonrpc_send, send_data);

    FinishedContext finished_context;
    void * finished_data = reinterpret_cast<void*>(&finished_context);
    wf_jsonrpc_proxy_invoke(proxy, &jsonrpc_finished, finished_data, "foo", "si", "bar", 42);

    ASSERT_TRUE(send_context.is_called);
    ASSERT_TRUE(json_is_object(send_context.response));

    ASSERT_FALSE(finished_context.is_called);

    wf_jsonrpc_proxy_dispose(proxy);

    ASSERT_TRUE(finished_context.is_called);

    wf_timer_manager_dispose(timer_manager);
}



TEST(wf_jsonrpc_proxy, notify)
{
    struct wf_timer_manager * timer_manager = wf_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wf_jsonrpc_proxy * proxy = wf_jsonrpc_proxy_create(timer_manager, WF_DEFAULT_TIMEOUT, &jsonrpc_send, send_data);

    wf_jsonrpc_proxy_notify(proxy, "foo", "si", "bar", 42);

    ASSERT_TRUE(send_context.is_called);
    ASSERT_TRUE(json_is_object(send_context.response));

    json_t * method = json_object_get(send_context.response, "method");
    ASSERT_TRUE(json_is_string(method));
    ASSERT_STREQ("foo", json_string_value(method));

    json_t * params = json_object_get(send_context.response, "params");
    ASSERT_TRUE(json_is_array(params));
    ASSERT_EQ(2, json_array_size(params));
    ASSERT_TRUE(json_is_string(json_array_get(params, 0)));
    ASSERT_STREQ("bar", json_string_value(json_array_get(params, 0)));
    ASSERT_TRUE(json_is_integer(json_array_get(params, 1)));
    ASSERT_EQ(42, json_integer_value(json_array_get(params, 1)));

    json_t * id = json_object_get(send_context.response, "id");
    ASSERT_EQ(nullptr, id);

    wf_jsonrpc_proxy_dispose(proxy);
    wf_timer_manager_dispose(timer_manager);
}

TEST(wf_jsonrpc_proxy, notify_dont_send_invalid_request)
{
    struct wf_timer_manager * timer_manager = wf_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wf_jsonrpc_proxy * proxy = wf_jsonrpc_proxy_create(timer_manager, WF_DEFAULT_TIMEOUT, &jsonrpc_send, send_data);

    wf_jsonrpc_proxy_notify(proxy, "foo", "?");

    ASSERT_FALSE(send_context.is_called);

    wf_jsonrpc_proxy_dispose(proxy);
    wf_timer_manager_dispose(timer_manager);
}
