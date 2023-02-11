#ifndef WEBFUSE_COMMANDLINE_READER_HPP
#define WEBFUSE_COMMANDLINE_READER_HPP

namespace webfuse
{

class commandline_reader
{
public:
    commandline_reader(int argc, char * argv[]);
    ~commandline_reader() = default;
    bool next();
    char const * current() const;
private:
    int current_;
    int argc_;
    char ** argv_;    
};

}

#endif
