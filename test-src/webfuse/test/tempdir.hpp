#ifndef WEBFUSE_TEMPDIR_HPP
#define WEBFUSE_TEMPDIR_HPP

#include <string>

namespace webfuse
{

class tempdir
{
public:
    tempdir();
    ~tempdir();
    std::string const name() const;
private:
    std::string path;

};

}

#endif
