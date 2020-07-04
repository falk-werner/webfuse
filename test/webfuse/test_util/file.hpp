#ifndef WF_TEST_INTEGRATION_FILE_HPP
#define WF_TEST_INTEGRATION_FILE_HPP

#include <string>

namespace webfuse_test
{

class File final
{
public:
    explicit File(std::string const& path);
    ~File();
    bool isFile();
    bool isDirectory();
    bool hasAccessRights(int accessRights);
    bool hasSize(size_t size);
    bool hasSubdirectory(std::string const & subdir);
    bool hasContents(std::string const & contents);
private:
    std::string path_;
};

}

#endif
