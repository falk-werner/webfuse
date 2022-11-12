#include "webfuse/webfuse.hpp"

namespace webfuse
{

class app::detail
{
    int dummy;
};

app::app(int argc, char * argv[])
: d(new detail)
{

}

app::~app()
{
    delete d;
}

int app::run()
{
    return 0;
}

}