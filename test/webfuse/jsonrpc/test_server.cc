#include <gtest/gtest.h>
#include "webfuse/impl/jsonrpc/server.h"
#include "webfuse/impl/jsonrpc/request.h"
#include "webfuse/impl/json/node.h"
#include "webfuse/impl/message.h"
#include "webfuse/status.h"

#include "webfuse/test_util/json_doc.hpp"

using webfuse_test::JsonDoc;

namespace
{
    class Context
    {
    public:
        Context()
        : doc("{}")
        , response(nullptr)
        , is_called(false)
        {

        }

        JsonDoc doc;
        wf_json const * response;
        bool is_called;
    };

    bool jsonrpc_send(
	    wf_message * request,
        void * user_data)
    {
        Context * context = reinterpret_cast<Context*>(user_data);
        context->is_called = true;
        context->doc = std::move(JsonDoc(std::string(request->data, request->length)));
        context->response = context->doc.root();

        wf_impl_message_dispose(request);
        return true;
    }

    void sayHello(
        struct wf_jsonrpc_request * request,
        char const * method_name,
        wf_json const * params,
        void * user_data)
    {
        (void) method_name;
        (void) params;
        (void) user_data;

        wf_impl_jsonrpc_respond(request);
    }

}

TEST(wf_jsonrpc_server, process_request)
{
    struct wf_jsonrpc_server * server = wf_impl_jsonrpc_server_create();
    wf_impl_jsonrpc_server_add(server, "sayHello", &sayHello, nullptr);

    Context context;
    void * user_data = reinterpret_cast<void*>(&context);

    JsonDoc request("{\"method\": \"sayHello\", \"params\": [], \"id\": 23}");
    wf_impl_jsonrpc_server_process(server, request.root(), &jsonrpc_send, user_data);

    ASSERT_TRUE(context.is_called);
    ASSERT_NE(nullptr, context.response);
    ASSERT_TRUE(wf_impl_json_is_object(context.response));

    struct wf_json const * id = wf_impl_json_object_get(context.response, "id");
    ASSERT_TRUE(wf_impl_json_is_int(id));
    ASSERT_EQ(23, wf_impl_json_int_get(id));

    struct wf_json const * result = wf_impl_json_object_get(context.response, "result");
    ASSERT_TRUE(wf_impl_json_is_object(result));

    wf_impl_jsonrpc_server_dispose(server); 
}

TEST(wf_jsonrpc_server, process_request_with_object_params)
{
    struct wf_jsonrpc_server * server = wf_impl_jsonrpc_server_create();
    wf_impl_jsonrpc_server_add(server, "sayHello", &sayHello, nullptr);

    Context context;
    void * user_data = reinterpret_cast<void*>(&context);
    JsonDoc request("{\"method\": \"sayHello\", \"params\": {}, \"id\": 23}");
    wf_impl_jsonrpc_server_process(server, request.root(), &jsonrpc_send, user_data);

    ASSERT_TRUE(context.is_called);
    ASSERT_NE(nullptr, context.response);
    ASSERT_TRUE(wf_impl_json_is_object(context.response));

    struct wf_json const * id = wf_impl_json_object_get(context.response, "id");
    ASSERT_TRUE(wf_impl_json_is_int(id));
    ASSERT_EQ(23, wf_impl_json_int_get(id));

    struct wf_json const * result = wf_impl_json_object_get(context.response, "result");
    ASSERT_TRUE(wf_impl_json_is_object(result));

    wf_impl_jsonrpc_server_dispose(server); 
}

TEST(wf_jsonrpc_server, invoke_unknown_method)
{
    struct wf_jsonrpc_server * server = wf_impl_jsonrpc_server_create();
    wf_impl_jsonrpc_server_add(server, "sayHello", &sayHello, nullptr);

    Context context;
    void * user_data = reinterpret_cast<void*>(&context);
    JsonDoc request("{\"method\": \"greet\", \"params\": [], \"id\": 42}");
    wf_impl_jsonrpc_server_process(server, request.root(), &jsonrpc_send, user_data);

    ASSERT_TRUE(context.is_called);
    ASSERT_NE(nullptr, context.response);
    ASSERT_TRUE(wf_impl_json_is_object(context.response));

    struct wf_json const * id = wf_impl_json_object_get(context.response, "id");
    ASSERT_TRUE(wf_impl_json_is_int(id));
    ASSERT_EQ(42, wf_impl_json_int_get(id));

    struct wf_json const * err = wf_impl_json_object_get(context.response, "error");
    ASSERT_TRUE(wf_impl_json_is_object(err));

    struct wf_json const * err_code = wf_impl_json_object_get(err, "code");
    ASSERT_TRUE(wf_impl_json_is_int(err_code));
    ASSERT_EQ(WF_BAD_NOTIMPLEMENTED, wf_impl_json_int_get(err_code));

    struct wf_json const * err_message = wf_impl_json_object_get(err, "message");
    ASSERT_TRUE(wf_impl_json_is_string(err_message));

    wf_impl_jsonrpc_server_dispose(server); 
}

TEST(wf_jsonrpc_server, skip_invalid_request_missing_id)
{
    struct wf_jsonrpc_server * server = wf_impl_jsonrpc_server_create();

    Context context;
    void * user_data = reinterpret_cast<void*>(&context);
    JsonDoc request("{\"method\": \"sayHello\", \"params\": []}");
    wf_impl_jsonrpc_server_process(server, request.root(), &jsonrpc_send, user_data);

    ASSERT_FALSE(context.is_called);

    wf_impl_jsonrpc_server_dispose(server); 
}

TEST(wf_jsonrpc_server, skip_invalid_request_wrong_id_type)
{
    struct wf_jsonrpc_server * server = wf_impl_jsonrpc_server_create();

    Context context;
    void * user_data = reinterpret_cast<void*>(&context);
    JsonDoc request("{\"method\": \"sayHello\", \"params\": [], \"id\": \"42\"}");
    wf_impl_jsonrpc_server_process(server, request.root(), &jsonrpc_send, user_data);

    ASSERT_FALSE(context.is_called);

    wf_impl_jsonrpc_server_dispose(server); 
}

TEST(wf_jsonrpc_server, skip_invalid_request_missing_params)
{
    struct wf_jsonrpc_server * server = wf_impl_jsonrpc_server_create();

    Context context;
    void * user_data = reinterpret_cast<void*>(&context);
    JsonDoc request("{\"method\": \"sayHello\", \"id\": 23}");
    wf_impl_jsonrpc_server_process(server, request.root(), &jsonrpc_send, user_data);

    ASSERT_FALSE(context.is_called);

    wf_impl_jsonrpc_server_dispose(server); 
}

TEST(wf_jsonrpc_server, skip_invalid_request_wrong_params_type)
{
    struct wf_jsonrpc_server * server = wf_impl_jsonrpc_server_create();

    Context context;
    void * user_data = reinterpret_cast<void*>(&context);
    JsonDoc request("{\"method\": \"sayHello\", \"params\": \"invalid\", \"id\": 42}");
    wf_impl_jsonrpc_server_process(server, request.root(), &jsonrpc_send, user_data);

    ASSERT_FALSE(context.is_called);

    wf_impl_jsonrpc_server_dispose(server); 
}

TEST(wf_jsonrpc_server, skip_invalid_request_missing_method)
{
    struct wf_jsonrpc_server * server = wf_impl_jsonrpc_server_create();

    Context context;
    void * user_data = reinterpret_cast<void*>(&context);
    JsonDoc request("{\"params\": [], \"id\": 23}");
    wf_impl_jsonrpc_server_process(server, request.root(), &jsonrpc_send, user_data);

    ASSERT_FALSE(context.is_called);

    wf_impl_jsonrpc_server_dispose(server); 
}

TEST(wf_jsonrpc_server, skip_invalid_request_wront_method_type)
{
    struct wf_jsonrpc_server * server = wf_impl_jsonrpc_server_create();

    Context context;
    void * user_data = reinterpret_cast<void*>(&context);
    JsonDoc request("{\"method\": 42, \"params\": [], \"id\": 23}");
    wf_impl_jsonrpc_server_process(server, request.root(), &jsonrpc_send, user_data);

    ASSERT_FALSE(context.is_called);

    wf_impl_jsonrpc_server_dispose(server); 
}