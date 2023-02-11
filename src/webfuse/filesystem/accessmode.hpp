#ifndef WEBFUSE_ACCESSMODE_HPP
#define WEBFUSE_ACCESSMODE_HPP

#include <cinttypes>

namespace webfuse
{

class access_mode
{
public:
    static constexpr int8_t const f_ok = 0; // F_OK
    static constexpr int8_t const r_ok = 4; // R_OK
    static constexpr int8_t const w_ok = 2; // W_OK
    static constexpr int8_t const x_ok = 1; // X_OK

    access_mode(int8_t value = f_ok);
    operator int8_t() const;
 
    static access_mode from_int(int value);
    int to_int() const;
private:
    int8_t value_;
};

}

#endif
