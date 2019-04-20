#include "mock_request.hpp"
#include <cstdlib>

namespace
{

extern "C" void
respond(
    json_t * response,
    void * user_data)
{
    (void) response;

    webfuse_test::Request * request = reinterpret_cast<webfuse_test::Request*>(user_data);
    request->respond_error(nullptr, 0);
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
}



}