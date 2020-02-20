#include "webfuse/utils/die_if.hpp"
#include <cstdlib>

namespace webfuse_test
{

void die_if(bool expression)
{
    if (expression)
    {
        exit(EXIT_FAILURE);
    }
}
   
}