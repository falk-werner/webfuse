#include "webfuse/impl/jsonrpc/request.h"
#include "webfuse/impl/json/node.h"
#include "webfuse/impl/message.h"
#include "webfuse/status.h"
#include "webfuse/test_util/json_doc.hpp"

#include <gtest/gtest.h>

using webfuse_test::JsonDoc;

namespace
{

struct Context
{
    std::string response;
};

bool jsonrpc_send(
	wf_message * request,
    void * user_data)
{
   Context * context = reinterpret_cast<Context*>(user_data);
   context->response = std::string(request->data, request->length);

    wf_impl_message_dispose(request);
   return true;
}

}

TEST(wf_jsonrpc_request, create_dispose)
{
    Context context;
    void * user_data = reinterpret_cast<void*>(&context);

    struct  wf_jsonrpc_request * request = 
            wf_impl_jsonrpc_request_create(42, &jsonrpc_send, user_data);

    ASSERT_NE(nullptr, request);
    ASSERT_EQ(user_data, wf_impl_jsonrpc_request_get_userdata(request));

    wf_impl_jsonrpc_request_dispose(request);
}

TEST(wf_jsonrpc_request, respond)
{
    Context context;
    void * user_data = reinterpret_cast<void*>(&context);

    struct  wf_jsonrpc_request * request = 
            wf_impl_jsonrpc_request_create(42, &jsonrpc_send, user_data);

    wf_impl_jsonrpc_respond(request);

    JsonDoc doc(context.response);
    struct wf_json const * response = doc.root();
    ASSERT_TRUE(wf_impl_json_is_object(response));
    
    struct wf_json const * id = wf_impl_json_object_get(response, "id");
    ASSERT_TRUE(wf_impl_json_is_int(id));
    ASSERT_EQ(42, wf_impl_json_int_get(id));

    struct wf_json const * result = wf_impl_json_object_get(response, "result");
    ASSERT_TRUE(wf_impl_json_is_object(result));

    ASSERT_TRUE(wf_impl_json_is_undefined(wf_impl_json_object_get(response, "error")));
}

TEST(wf_jsonrpc_request, respond_error)
{
    Context context;
    void * user_data = reinterpret_cast<void*>(&context);

    struct  wf_jsonrpc_request * request = 
            wf_impl_jsonrpc_request_create(42, &jsonrpc_send, user_data);

    wf_impl_jsonrpc_respond_error(request, WF_BAD, "Bad");

    JsonDoc doc(context.response);
    struct wf_json const * response = doc.root();
    ASSERT_TRUE(wf_impl_json_is_object(response));
    
    struct wf_json const * id = wf_impl_json_object_get(response, "id");
    ASSERT_TRUE(wf_impl_json_is_int(id));
    ASSERT_EQ(42, wf_impl_json_int_get(id));

    ASSERT_TRUE(wf_impl_json_is_undefined(wf_impl_json_object_get(response, "result")));

    struct wf_json const * err = wf_impl_json_object_get(response, "error");
    ASSERT_TRUE(wf_impl_json_is_object(err));

    struct wf_json const * err_code = wf_impl_json_object_get(err, "code");
    ASSERT_TRUE(wf_impl_json_is_int(err_code));
    ASSERT_EQ(WF_BAD, wf_impl_json_int_get(err_code));

    struct wf_json const * err_message = wf_impl_json_object_get(err, "message");
    ASSERT_TRUE(wf_impl_json_is_string(err_message));
    ASSERT_STREQ("Bad", wf_impl_json_string_get(err_message));
}
