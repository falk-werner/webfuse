#ifndef WEBFUSE_FILEHANDLE_HPP
#define WEBFUSE_FILEHANDLE_HPP

#include <cinttypes>

namespace webfuse
{

using filehandle = uint64_t;

constexpr filehandle const invalid_handle = (filehandle) -1;


}

#endif
