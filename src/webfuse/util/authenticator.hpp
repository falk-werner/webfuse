#ifndef WEBFUSE_AUTHENTICATOR_HPP
#define WEBFUSE_AUTHENTICATOR_HPP

#include <string>

namespace webfuse
{

class authenticator
{
public:
    explicit authenticator(std::string const & app);
    ~authenticator() = default;

    bool authenticate(std::string const & token);

private:
    std::string app_;
};

}

#endif
