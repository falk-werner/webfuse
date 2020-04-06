#include <gtest/gtest.h>
#include "webfuse/core/jsonrpc/server.h"
#include "webfuse/core/jsonrpc/request.h"
#include "webfuse/core/status.h"

namespace
{
    struct Context
    {
        json_t * response;
        bool is_called;
    };

    bool jsonrpc_send(
	    json_t * request,
        void * user_data)
    {
        Context * context = reinterpret_cast<Context*>(user_data);
        context->is_called = true;
        context->response = request;
        json_incref(request);

        return true;
    }

    void sayHello(
        struct wf_jsonrpc_request * request,
        char const * method_name,
        json_t * params,
        void * user_data)
    {
        (void) method_name;
        (void) params;
        (void) user_data;

        json_t * result = json_string("Hello");
        wf_jsonrpc_respond(request, result);
    }

}

TEST(wf_jsonrpc_server, process_request)
{
    struct wf_jsonrpc_server * server = wf_jsonrpc_server_create();
    wf_jsonrpc_server_add(server, "sayHello", &sayHello, nullptr);

    Context context{nullptr, false};
    void * user_data = reinterpret_cast<void*>(&context);
    json_t * request = json_object();
    json_object_set_new(request, "method", json_string("sayHello"));
    json_object_set_new(request, "params", json_array());
    json_object_set_new(request, "id", json_integer(23));   
    wf_jsonrpc_server_process(server, request, &jsonrpc_send, user_data);

    ASSERT_TRUE(context.is_called);
    ASSERT_NE(nullptr, context.response);
    ASSERT_TRUE(json_is_object(context.response));

    json_t * id = json_object_get(context.response, "id");
    ASSERT_TRUE(json_is_integer(id));
    ASSERT_EQ(23, json_integer_value(id));

    json_t * result = json_object_get(context.response, "result");
    ASSERT_TRUE(json_is_string(result));
    ASSERT_STREQ("Hello", json_string_value(result));

    json_decref(context.response);
    json_decref(request);
    wf_jsonrpc_server_dispose(server); 
}

TEST(wf_jsonrpc_server, process_request_with_oject_params)
{
    struct wf_jsonrpc_server * server = wf_jsonrpc_server_create();
    wf_jsonrpc_server_add(server, "sayHello", &sayHello, nullptr);

    Context context{nullptr, false};
    void * user_data = reinterpret_cast<void*>(&context);
    json_t * request = json_object();
    json_object_set_new(request, "method", json_string("sayHello"));
    json_object_set_new(request, "params", json_object());
    json_object_set_new(request, "id", json_integer(23));   
    wf_jsonrpc_server_process(server, request, &jsonrpc_send, user_data);

    ASSERT_TRUE(context.is_called);
    ASSERT_NE(nullptr, context.response);
    ASSERT_TRUE(json_is_object(context.response));

    json_t * id = json_object_get(context.response, "id");
    ASSERT_TRUE(json_is_integer(id));
    ASSERT_EQ(23, json_integer_value(id));

    json_t * result = json_object_get(context.response, "result");
    ASSERT_TRUE(json_is_string(result));
    ASSERT_STREQ("Hello", json_string_value(result));

    json_decref(context.response);
    json_decref(request);
    wf_jsonrpc_server_dispose(server); 
}

TEST(wf_jsonrpc_server, invoke_unknown_method)
{
    struct wf_jsonrpc_server * server = wf_jsonrpc_server_create();
    wf_jsonrpc_server_add(server, "sayHello", &sayHello, nullptr);

    Context context{nullptr, false};
    void * user_data = reinterpret_cast<void*>(&context);
    json_t * request = json_object();
    json_object_set_new(request, "method", json_string("greet"));
    json_object_set_new(request, "params", json_array());
    json_object_set_new(request, "id", json_integer(42));   
    wf_jsonrpc_server_process(server, request, &jsonrpc_send, user_data);

    ASSERT_TRUE(context.is_called);
    ASSERT_NE(nullptr, context.response);
    ASSERT_TRUE(json_is_object(context.response));

    json_t * id = json_object_get(context.response, "id");
    ASSERT_TRUE(json_is_integer(id));
    ASSERT_EQ(42, json_integer_value(id));

    json_t * err = json_object_get(context.response, "error");
    ASSERT_TRUE(json_is_object(err));

    json_t * err_code = json_object_get(err, "code");
    ASSERT_TRUE(json_is_integer(err_code));
    ASSERT_EQ(WF_BAD_NOTIMPLEMENTED, json_integer_value(err_code));

    json_t * err_message = json_object_get(err, "message");
    ASSERT_TRUE(json_is_string(err_message));

    json_decref(context.response);
    json_decref(request);
    wf_jsonrpc_server_dispose(server); 
}

TEST(wf_jsonrpc_server, skip_invalid_request_missing_id)
{
    struct wf_jsonrpc_server * server = wf_jsonrpc_server_create();

    Context context{nullptr, false};
    void * user_data = reinterpret_cast<void*>(&context);
    json_t * request = json_object();
    json_object_set_new(request, "method", json_string("sayHello"));
    json_object_set_new(request, "params", json_array());
    wf_jsonrpc_server_process(server, request, &jsonrpc_send, user_data);

    ASSERT_FALSE(context.is_called);

    json_decref(request);
    wf_jsonrpc_server_dispose(server); 
}

TEST(wf_jsonrpc_server, skip_invalid_request_wrong_id_type)
{
    struct wf_jsonrpc_server * server = wf_jsonrpc_server_create();

    Context context{nullptr, false};
    void * user_data = reinterpret_cast<void*>(&context);
    json_t * request = json_object();
    json_object_set_new(request, "method", json_string("sayHello"));
    json_object_set_new(request, "params", json_array());
    json_object_set_new(request, "id", json_string("42"));   
    wf_jsonrpc_server_process(server, request, &jsonrpc_send, user_data);

    ASSERT_FALSE(context.is_called);

    json_decref(request);
    wf_jsonrpc_server_dispose(server); 
}

TEST(wf_jsonrpc_server, skip_invalid_request_missing_params)
{
    struct wf_jsonrpc_server * server = wf_jsonrpc_server_create();

    Context context{nullptr, false};
    void * user_data = reinterpret_cast<void*>(&context);
    json_t * request = json_object();
    json_object_set_new(request, "method", json_string("sayHello"));
    json_object_set_new(request, "id", json_integer(42));   
    wf_jsonrpc_server_process(server, request, &jsonrpc_send, user_data);

    ASSERT_FALSE(context.is_called);

    json_decref(request);
    wf_jsonrpc_server_dispose(server); 
}

TEST(wf_jsonrpc_server, skip_invalid_request_wrong_params_type)
{
    struct wf_jsonrpc_server * server = wf_jsonrpc_server_create();

    Context context{nullptr, false};
    void * user_data = reinterpret_cast<void*>(&context);
    json_t * request = json_object();
    json_object_set_new(request, "method", json_string("sayHello"));
    json_object_set_new(request, "params", json_string("invalid"));
    json_object_set_new(request, "id", json_integer(42));   
    wf_jsonrpc_server_process(server, request, &jsonrpc_send, user_data);

    ASSERT_FALSE(context.is_called);

    json_decref(request);
    wf_jsonrpc_server_dispose(server); 
}

TEST(wf_jsonrpc_server, skip_invalid_request_missing_method)
{
    struct wf_jsonrpc_server * server = wf_jsonrpc_server_create();

    Context context{nullptr, false};
    void * user_data = reinterpret_cast<void*>(&context);
    json_t * request = json_object();
    json_object_set_new(request, "params", json_array());
    json_object_set_new(request, "id", json_integer(42));   
    wf_jsonrpc_server_process(server, request, &jsonrpc_send, user_data);

    ASSERT_FALSE(context.is_called);

    json_decref(request);
    wf_jsonrpc_server_dispose(server); 
}

TEST(wf_jsonrpc_server, skip_invalid_request_wront_method_type)
{
    struct wf_jsonrpc_server * server = wf_jsonrpc_server_create();

    Context context{nullptr, false};
    void * user_data = reinterpret_cast<void*>(&context);
    json_t * request = json_object();
    json_object_set_new(request, "method", json_integer(42));
    json_object_set_new(request, "params", json_array());
    json_object_set_new(request, "id", json_integer(42));   
    wf_jsonrpc_server_process(server, request, &jsonrpc_send, user_data);

    ASSERT_FALSE(context.is_called);

    json_decref(request);
    wf_jsonrpc_server_dispose(server); 
}