#include "webfuse/filesystem/accessmode.hpp"
#include <unistd.h>

namespace webfuse
{

access_mode::access_mode(int8_t value)
: value_(value)
{

}

access_mode::operator int8_t() const
{
    return value_;
}

access_mode access_mode::from_int(int value)
{
    int8_t result = 0;

    if (0 != (value & R_OK)) { result |= r_ok; }
    if (0 != (value & W_OK)) { result |= w_ok; }
    if (0 != (value & X_OK)) { result |= x_ok; }

    return access_mode(result);
}

int access_mode::to_int() const
{
    int result = 0;

    if (0 != (value_ & r_ok)) { result |= R_OK; }
    if (0 != (value_ & w_ok)) { result |= W_OK; }
    if (0 != (value_ & x_ok)) { result |= X_OK; }
    
    return result;
}


}
