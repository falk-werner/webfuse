#include <gtest/gtest.h>
#include "webfuse/impl/jsonrpc/proxy.h"
#include "webfuse/impl/jsonrpc/error.h"
#include "webfuse/impl/json/node.h"
#include "webfuse/impl/message.h"
#include "webfuse/status.h"
#include "webfuse/impl/timer/manager.h"

#include "webfuse/jsonrpc/mock_timer.hpp"
#include "webfuse/test_util/json_doc.hpp"

#include <thread>
#include <chrono>

using namespace std::chrono_literals;
using wf_jsonrpc_test::MockTimer;
using webfuse_test::JsonDoc;
using testing::Return;
using testing::_;
using testing::DoAll;
using testing::SaveArg;

#define WF_DEFAULT_TIMEOUT (10 * 1000)

namespace
{
    struct SendContext
    {
        JsonDoc doc;
        wf_json const * response;
        bool result;
        bool is_called;

        explicit SendContext(bool result_ = true)
        : doc("null")
        , response(nullptr)
        , result(result_)
        , is_called(false)
        {
        }
    };

    bool jsonrpc_send(
        wf_message * request,
        void * user_data)
    {
        SendContext * context = reinterpret_cast<SendContext*>(user_data);
        context->is_called = true;
        context->doc = std::move(JsonDoc(std::string(request->data, request->length)));
        context->response = context->doc.root();

        wf_impl_message_dispose(request);
        return context->result;
    }

    struct FinishedContext
    {
        bool is_called;
        wf_jsonrpc_error * error;

        FinishedContext()
        : is_called(false)
        , error(nullptr)
        {

        }

        ~FinishedContext()
        {
            wf_impl_jsonrpc_error_dispose(error);
        }
    };

    void jsonrpc_finished(
        void * user_data,
        wf_json const *,
        wf_jsonrpc_error const * error)
    {
        FinishedContext * context = reinterpret_cast<FinishedContext*>(user_data);
        context->is_called = true;

        if (nullptr != error)
        {
            context->error = wf_impl_jsonrpc_error(wf_impl_jsonrpc_error_code(error),
                wf_impl_jsonrpc_error_message(error));
        }
    }
}

TEST(wf_jsonrpc_proxy, init)
{
    struct wf_timer_manager * timer_manager = wf_impl_timer_manager_create();

    SendContext context;
    void * user_data = reinterpret_cast<void*>(&context);
    struct wf_jsonrpc_proxy * proxy = wf_impl_jsonrpc_proxy_create(timer_manager, WF_DEFAULT_TIMEOUT, &jsonrpc_send, user_data);

    wf_impl_jsonrpc_proxy_dispose(proxy);
    wf_impl_timer_manager_dispose(timer_manager);

    ASSERT_FALSE(context.is_called);
}

TEST(wf_jsonrpc_proxy, invoke)
{
    struct wf_timer_manager * timer_manager = wf_impl_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wf_jsonrpc_proxy * proxy = wf_impl_jsonrpc_proxy_create(timer_manager, WF_DEFAULT_TIMEOUT, &jsonrpc_send, send_data);

    FinishedContext finished_context;
    void * finished_data = reinterpret_cast<void*>(&finished_context);
    wf_impl_jsonrpc_proxy_invoke(proxy, &jsonrpc_finished, finished_data, "foo", "si", "bar", 42);

    ASSERT_TRUE(send_context.is_called);
    ASSERT_TRUE(wf_impl_json_is_object(send_context.response));

    wf_json const * method = wf_impl_json_object_get(send_context.response, "method");
    ASSERT_TRUE(wf_impl_json_is_string(method));
    ASSERT_STREQ("foo", wf_impl_json_string_get(method));

    wf_json const * params = wf_impl_json_object_get(send_context.response, "params");
    ASSERT_TRUE(wf_impl_json_is_array(params));
    ASSERT_EQ(2, wf_impl_json_array_size(params));
    ASSERT_TRUE(wf_impl_json_is_string(wf_impl_json_array_get(params, 0)));
    ASSERT_STREQ("bar", wf_impl_json_string_get(wf_impl_json_array_get(params, 0)));
    ASSERT_TRUE(wf_impl_json_is_int(wf_impl_json_array_get(params, 1)));
    ASSERT_EQ(42, wf_impl_json_int_get(wf_impl_json_array_get(params, 1)));

    wf_json const * id = wf_impl_json_object_get(send_context.response, "id");
    ASSERT_TRUE(wf_impl_json_is_int(id));

    ASSERT_FALSE(finished_context.is_called);

    wf_impl_jsonrpc_proxy_dispose(proxy);
    wf_impl_timer_manager_dispose(timer_manager);

    ASSERT_TRUE(finished_context.is_called);
    ASSERT_FALSE(nullptr == finished_context.error);
}

TEST(wf_jsonrpc_proxy, invoke_calls_finish_if_send_fails)
{
    struct wf_timer_manager * timer_manager = wf_impl_timer_manager_create();

    SendContext send_context(false);
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wf_jsonrpc_proxy * proxy = wf_impl_jsonrpc_proxy_create(timer_manager, WF_DEFAULT_TIMEOUT, &jsonrpc_send, send_data);

    FinishedContext finished_context;
    void * finished_data = reinterpret_cast<void*>(&finished_context);
    wf_impl_jsonrpc_proxy_invoke(proxy, &jsonrpc_finished, finished_data, "foo", "si", "bar", 42);

    ASSERT_TRUE(send_context.is_called);
    ASSERT_TRUE(wf_impl_json_is_object(send_context.response));

    ASSERT_TRUE(finished_context.is_called);
    ASSERT_FALSE(nullptr == finished_context.error);

    wf_impl_jsonrpc_proxy_dispose(proxy);
    wf_impl_timer_manager_dispose(timer_manager);
}

TEST(wf_jsonrpc_proxy, invoke_if_another_request_is_pending)
{
    struct wf_timer_manager * timer_manager = wf_impl_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wf_jsonrpc_proxy * proxy = wf_impl_jsonrpc_proxy_create(timer_manager, WF_DEFAULT_TIMEOUT, &jsonrpc_send, send_data);

    FinishedContext finished_context;
    void * finished_data = reinterpret_cast<void*>(&finished_context);
    wf_impl_jsonrpc_proxy_invoke(proxy, &jsonrpc_finished, finished_data, "foo", "si", "bar", 42);

    FinishedContext finished_context2;
    void * finished_data2 = reinterpret_cast<void*>(&finished_context2);
    wf_impl_jsonrpc_proxy_invoke(proxy, &jsonrpc_finished, finished_data2, "foo", "");

    ASSERT_TRUE(send_context.is_called);
    ASSERT_TRUE(wf_impl_json_is_object(send_context.response));

    ASSERT_FALSE(finished_context.is_called);

    wf_impl_jsonrpc_proxy_dispose(proxy);
    wf_impl_timer_manager_dispose(timer_manager);
}

TEST(wf_jsonrpc_proxy, invoke_invalid_request)
{
    struct wf_timer_manager * timer_manager = wf_impl_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wf_jsonrpc_proxy * proxy = wf_impl_jsonrpc_proxy_create(timer_manager, WF_DEFAULT_TIMEOUT, &jsonrpc_send, send_data);

    FinishedContext finished_context;
    void * finished_data = reinterpret_cast<void*>(&finished_context);
    wf_impl_jsonrpc_proxy_invoke(proxy, &jsonrpc_finished, finished_data, "foo", "?", "error");

    ASSERT_TRUE(send_context.is_called);

    ASSERT_FALSE(finished_context.is_called);

    wf_impl_jsonrpc_proxy_dispose(proxy);
    wf_impl_timer_manager_dispose(timer_manager);
}

TEST(wf_jsonrpc_proxy, on_result)
{
    struct wf_timer_manager * timer_manager = wf_impl_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wf_jsonrpc_proxy * proxy = wf_impl_jsonrpc_proxy_create(timer_manager, WF_DEFAULT_TIMEOUT, &jsonrpc_send, send_data);

    FinishedContext finished_context;
    void * finished_data = reinterpret_cast<void*>(&finished_context);
    wf_impl_jsonrpc_proxy_invoke(proxy, &jsonrpc_finished, finished_data, "foo", "si", "bar", 42);

    ASSERT_TRUE(send_context.is_called);
    ASSERT_TRUE(wf_impl_json_is_object(send_context.response));

    wf_json const * id = wf_impl_json_object_get(send_context.response, "id");
    ASSERT_TRUE(wf_impl_json_is_int(id));

    JsonDoc response("{\"result\": \"okay\", \"id\": " + std::to_string(wf_impl_json_int_get(id)) + "}");
    wf_impl_jsonrpc_proxy_onresult(proxy, response.root());

    ASSERT_TRUE(finished_context.is_called);
    ASSERT_EQ(nullptr, finished_context.error);

    wf_impl_jsonrpc_proxy_dispose(proxy);
    wf_impl_timer_manager_dispose(timer_manager);
}

TEST(wf_jsonrpc_proxy, on_result_reject_response_with_unknown_id)
{
    struct wf_timer_manager * timer_manager = wf_impl_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wf_jsonrpc_proxy * proxy = wf_impl_jsonrpc_proxy_create(timer_manager, WF_DEFAULT_TIMEOUT, &jsonrpc_send, send_data);

    FinishedContext finished_context;
    void * finished_data = reinterpret_cast<void*>(&finished_context);
    wf_impl_jsonrpc_proxy_invoke(proxy, &jsonrpc_finished, finished_data, "foo", "si", "bar", 42);

    ASSERT_TRUE(send_context.is_called);
    ASSERT_TRUE(wf_impl_json_is_object(send_context.response));

    wf_json const * id = wf_impl_json_object_get(send_context.response, "id");
    ASSERT_TRUE(wf_impl_json_is_int(id));

    JsonDoc response("{\"result\": \"okay\", \"id\": " + std::to_string(wf_impl_json_int_get(id) + 1) + "}");
    wf_impl_jsonrpc_proxy_onresult(proxy, response.root());

    ASSERT_FALSE(finished_context.is_called);

    wf_impl_jsonrpc_proxy_dispose(proxy);
    wf_impl_timer_manager_dispose(timer_manager);
}

TEST(wf_jsonrpc_proxy, timeout)
{
    struct wf_timer_manager * timer_manager = wf_impl_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wf_jsonrpc_proxy * proxy = wf_impl_jsonrpc_proxy_create(timer_manager, 0, &jsonrpc_send, send_data);

    FinishedContext finished_context;
    void * finished_data = reinterpret_cast<void*>(&finished_context);
    wf_impl_jsonrpc_proxy_invoke(proxy, &jsonrpc_finished, finished_data, "foo", "si", "bar", 42);

    ASSERT_TRUE(send_context.is_called);
    ASSERT_TRUE(wf_impl_json_is_object(send_context.response));

    std::this_thread::sleep_for(10ms);
    wf_impl_timer_manager_check(timer_manager);

    ASSERT_TRUE(finished_context.is_called);
    ASSERT_EQ(WF_BAD_TIMEOUT, wf_impl_jsonrpc_error_code(finished_context.error));

    wf_impl_jsonrpc_proxy_dispose(proxy);
    wf_impl_timer_manager_dispose(timer_manager);
}

TEST(wf_jsonrpc_proxy, cleanup_pending_request)
{
    struct wf_timer_manager * timer_manager = wf_impl_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wf_jsonrpc_proxy * proxy = wf_impl_jsonrpc_proxy_create(timer_manager, 10, &jsonrpc_send, send_data);

    FinishedContext finished_context;
    void * finished_data = reinterpret_cast<void*>(&finished_context);
    wf_impl_jsonrpc_proxy_invoke(proxy, &jsonrpc_finished, finished_data, "foo", "si", "bar", 42);

    ASSERT_TRUE(send_context.is_called);
    ASSERT_TRUE(wf_impl_json_is_object(send_context.response));

    ASSERT_FALSE(finished_context.is_called);

    wf_impl_jsonrpc_proxy_dispose(proxy);

    ASSERT_TRUE(finished_context.is_called);

    wf_impl_timer_manager_dispose(timer_manager);
}



TEST(wf_jsonrpc_proxy, notify)
{
    struct wf_timer_manager * timer_manager = wf_impl_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wf_jsonrpc_proxy * proxy = wf_impl_jsonrpc_proxy_create(timer_manager, WF_DEFAULT_TIMEOUT, &jsonrpc_send, send_data);

    wf_impl_jsonrpc_proxy_notify(proxy, "foo", "si", "bar", 42);

    ASSERT_TRUE(send_context.is_called);
    ASSERT_TRUE(wf_impl_json_is_object(send_context.response));

    wf_json const * method = wf_impl_json_object_get(send_context.response, "method");
    ASSERT_TRUE(wf_impl_json_is_string(method));
    ASSERT_STREQ("foo", wf_impl_json_string_get(method));

    wf_json const * params = wf_impl_json_object_get(send_context.response, "params");
    ASSERT_TRUE(wf_impl_json_is_array(params));
    ASSERT_EQ(2, wf_impl_json_array_size(params));
    ASSERT_TRUE(wf_impl_json_is_string(wf_impl_json_array_get(params, 0)));
    ASSERT_STREQ("bar", wf_impl_json_string_get(wf_impl_json_array_get(params, 0)));
    ASSERT_TRUE(wf_impl_json_is_int(wf_impl_json_array_get(params, 1)));
    ASSERT_EQ(42, wf_impl_json_int_get(wf_impl_json_array_get(params, 1)));

    wf_json const * id = wf_impl_json_object_get(send_context.response, "id");
    ASSERT_TRUE(wf_impl_json_is_undefined(id));

    wf_impl_jsonrpc_proxy_dispose(proxy);
    wf_impl_timer_manager_dispose(timer_manager);
}

TEST(wf_jsonrpc_proxy, notify_send_invalid_request)
{
    struct wf_timer_manager * timer_manager = wf_impl_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wf_jsonrpc_proxy * proxy = wf_impl_jsonrpc_proxy_create(timer_manager, WF_DEFAULT_TIMEOUT, &jsonrpc_send, send_data);

    wf_impl_jsonrpc_proxy_notify(proxy, "foo", "?");

    ASSERT_TRUE(send_context.is_called);

    wf_impl_jsonrpc_proxy_dispose(proxy);
    wf_impl_timer_manager_dispose(timer_manager);
}

TEST(wf_jsonrpc_proxy, on_result_swallow_if_no_request_pending)
{
    struct wf_timer_manager * timer_manager = wf_impl_timer_manager_create();

    SendContext send_context;
    void * send_data = reinterpret_cast<void*>(&send_context);
    struct wf_jsonrpc_proxy * proxy = wf_impl_jsonrpc_proxy_create(timer_manager, WF_DEFAULT_TIMEOUT, &jsonrpc_send, send_data);

    JsonDoc response("{\"result\": \"okay\", \"id\": 42}");
    wf_impl_jsonrpc_proxy_onresult(proxy, response.root());

    wf_impl_jsonrpc_proxy_dispose(proxy);
    wf_impl_timer_manager_dispose(timer_manager);
}

