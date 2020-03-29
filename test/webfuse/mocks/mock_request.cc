#include "webfuse/mocks/mock_request.hpp"
#include <cstdlib>

extern "C" 
{

static void webfuse_test_MockRequest_respond(
    json_t * response,
    void * user_data)
{
    auto * request = reinterpret_cast<webfuse_test::MockRequest*>(user_data);

    json_t * result = json_object_get(response, "result");
    json_t * error = json_object_get(response, "error");
    json_t * id_holder = json_object_get(response, "id");

    int id = -1;
    if (json_is_integer(id_holder)) 
    {
        id = json_integer_value(id_holder);
    }

    if (nullptr != result)
    {
        request->respond(result, id);
    }
    else
    {
        request->respond_error(error, id);
    }
}

}

namespace webfuse_test
{

struct wfp_request * MockRequest::create_request(int id)
{
    auto * request = reinterpret_cast<wfp_request*>(malloc(sizeof(wfp_request)));
    request->respond   = &webfuse_test_MockRequest_respond;
    request->user_data = reinterpret_cast<void*>(this);
    request->id = id;

    return request;
}



}