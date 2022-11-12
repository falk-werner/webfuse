#include "webfuse/webfuse.hpp"

int main(int argc, char* argv[])
{
    webfuse::app app(argc, argv);
    return app.run();
}