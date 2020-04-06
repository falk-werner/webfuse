#include "webfuse/mocks/fake_invokation_context.hpp"

namespace webfuse_test
{

wfp_impl_invokation_context create_context(MockProvider& provider, wfp_request * request)
{
    wfp_impl_invokation_context context =
    {
        provider.get_provider(),
        provider.get_userdata(),
        request
    };
    return context;
}

}