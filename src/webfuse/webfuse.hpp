#ifndef WEBFUSE_HPP
#define WEBFUSE_HPP

namespace webfuse
{

class app
{
    app(app const &) = delete;
    app& operator=(app const &) = delete;
    app(app &&) = delete;
    app& operator=(app &&) = delete;
public:
    app(int argc, char * argv[]);
    ~app();
    int run();
private:
    class detail;
    detail * d;
};

}

#endif