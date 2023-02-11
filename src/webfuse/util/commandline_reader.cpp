#include "webfuse/util/commandline_reader.hpp"
namespace webfuse
{

commandline_reader::commandline_reader(int argc, char * argv[])
: current_(0)
, argc_(argc)
, argv_(argv)
{

}

bool commandline_reader::next()
{
    if (current_ < argc_)
    {
        current_++;
    }

    bool const has_next = (current_ < argc_);
    return has_next;
}

char const * commandline_reader::current() const
{
    return argv_[current_];
}


}
