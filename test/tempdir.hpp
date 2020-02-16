#ifndef WF_TEST_TEMPDIR_HPP
#define WF_TEST_TEMPDIR_HPP

namespace webfuse_test
{

class TempDir
{
    TempDir(TempDir const &) = delete;
    TempDir & operator=(TempDir const &) = delete;
public:
    explicit TempDir(char const * prefix);
    ~TempDir();
    char const * path();
private:
    char * path_;
};

}

#endif
