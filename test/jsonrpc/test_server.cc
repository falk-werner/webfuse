#include <gtest/gtest.h>
#include "jsonrpc/server.h"
#include "jsonrpc/request.h"

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
        struct jsonrpc_request * request,
        char const * method_name,
        json_t * params,
        void * user_data)
    {
        (void) method_name;
        (void) params;
        (void) user_data;

        json_t * result = json_string("Hello");
        jsonrpc_respond(request, result);
    }

}

TEST(jsonrpc_server, process_request)
{
    struct jsonrpc_server server;
    jsonrpc_server_init(&server);
    jsonrpc_server_add(&server, "sayHello", &sayHello, nullptr);

    Context context{nullptr, false};
    void * user_data = reinterpret_cast<void*>(&context);
    json_t * request = json_object();
    json_object_set_new(request, "method", json_string("sayHello"));
    json_object_set_new(request, "params", json_array());
    json_object_set_new(request, "id", json_integer(23));   
    jsonrpc_server_process(&server, request, &jsonrpc_send, user_data);

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
    jsonrpc_server_cleanup(&server); 
}

TEST(jsonrpc_server, invoke_unknown_method)
{
    struct jsonrpc_server server;
    jsonrpc_server_init(&server);
    jsonrpc_server_add(&server, "sayHello", &sayHello, nullptr);

    Context context{nullptr, false};
    void * user_data = reinterpret_cast<void*>(&context);
    json_t * request = json_object();
    json_object_set_new(request, "method", json_string("greet"));
    json_object_set_new(request, "params", json_array());
    json_object_set_new(request, "id", json_integer(42));   
    jsonrpc_server_process(&server, request, &jsonrpc_send, user_data);

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
    jsonrpc_server_cleanup(&server); 
}

TEST(jsonrpc_server, skip_invalid_request)
{
    struct jsonrpc_server server;
    jsonrpc_server_init(&server);

    Context context{nullptr, false};
    void * user_data = reinterpret_cast<void*>(&context);
    json_t * request = json_object();
    json_object_set_new(request, "method", json_string("sayHello"));
    json_object_set_new(request, "params", json_array());
    jsonrpc_server_process(&server, request, &jsonrpc_send, user_data);

    ASSERT_FALSE(context.is_called);

    json_decref(request);
    jsonrpc_server_cleanup(&server); 
}
