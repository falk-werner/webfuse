#include "webfuse/ws/config.hpp"

namespace
{

constexpr int const default_port = 8081;

}

namespace webfuse
{

ws_config::ws_config()
: port(default_port)
{

}


}