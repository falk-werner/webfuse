#include "webfuse/mocks/mock_request.hpp"
#include <cstdlib>

namespace
{

extern "C" void
respond(
    json_t * response,
    void * user_data)
{
    webfuse_test::Request * request = reinterpret_cast<webfuse_test::Request*>(user_data);

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

struct wfp_request *
request_create(
    Request * req,
    int id)
{
    struct wfp_request * request = reinterpret_cast<struct wfp_request *>(malloc(sizeof(struct wfp_request)));
    request->id = id;
    request->user_data = reinterpret_cast<void*>(req);
    request->respond = &respond;

    return request;
}



}