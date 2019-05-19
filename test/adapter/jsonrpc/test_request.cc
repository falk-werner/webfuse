#include <gtest/gtest.h>
#include "webfuse/adapter/impl/jsonrpc/request.h"

namespace
{

struct Context
{
    json_t * response;
};

bool jsonrpc_send(
	json_t * request,
    void * user_data)
{
   Context * context = reinterpret_cast<Context*>(user_data);
   context->response = request;
   json_incref(request);

   return true;
}

}

TEST(jsonrpc_request, create_dispose)
{
    Context context{nullptr};
    void * user_data = reinterpret_cast<void*>(&context);

    struct  wf_impl_jsonrpc_request * request = 
            wf_impl_jsonrpc_request_create(42, &jsonrpc_send, user_data);

    ASSERT_NE(nullptr, request);
    ASSERT_EQ(user_data, wf_impl_jsonrpc_request_get_userdata(request));

    wf_impl_jsonrpc_request_dispose(request);
}

TEST(jsonrpc_request, respond)
{
    Context context{nullptr};
    void * user_data = reinterpret_cast<void*>(&context);

    struct  wf_impl_jsonrpc_request * request = 
            wf_impl_jsonrpc_request_create(42, &jsonrpc_send, user_data);

    wf_impl_jsonrpc_respond(request, json_string("okay"));

    ASSERT_NE(nullptr, context.response);


    json_t * response = reinterpret_cast<json_t*>(context.response);
    ASSERT_TRUE(json_is_object(response));
    
    json_t * id = json_object_get(response, "id");
    ASSERT_TRUE(json_is_integer(id));
    ASSERT_EQ(42, json_integer_value(id));

    json_t * result = json_object_get(response, "result");
    ASSERT_TRUE(json_is_string(result));
    ASSERT_STREQ("okay", json_string_value(result));

    ASSERT_EQ(nullptr, json_object_get(response, "error"));

    json_decref(response);
}

TEST(jsonrpc_request, respond_error)
{
    Context context{nullptr};
    void * user_data = reinterpret_cast<void*>(&context);

    struct  wf_impl_jsonrpc_request * request = 
            wf_impl_jsonrpc_request_create(42, &jsonrpc_send, user_data);

    wf_impl_jsonrpc_respond_error(request, WF_BAD);

    ASSERT_NE(nullptr, context.response);


    json_t * response = reinterpret_cast<json_t*>(context.response);
    ASSERT_TRUE(json_is_object(response));
    
    json_t * id = json_object_get(response, "id");
    ASSERT_TRUE(json_is_integer(id));
    ASSERT_EQ(42, json_integer_value(id));

    ASSERT_EQ(nullptr, json_object_get(response, "result"));

    json_t * err = json_object_get(response, "error");
    ASSERT_TRUE(json_is_object(err));

    json_t * err_code = json_object_get(err, "code");
    ASSERT_TRUE(json_is_integer(err_code));
    ASSERT_EQ(WF_BAD, json_integer_value(err_code));

    json_t * err_message = json_object_get(err, "message");
    ASSERT_TRUE(json_is_string(err_message));
    ASSERT_STREQ("Bad", json_string_value(err_message));

    json_decref(response);
}
