#include "webfuse/tests/integration/file.hpp"
#include <cstdio>
#include <sstream>

namespace
{

bool invoke(std::string const & command)
{
    int exit_code = -1;

    FILE * file = ::popen(command.c_str(), "r");
    if (nullptr != file)
    {
        exit_code = ::pclose(file);
    }

    return (0 == exit_code);
}

}

namespace webfuse_test
{

File::File(std::string const& path)
: path_(path)
{

}

File::~File()
{

}

bool File::isFile()
{
    std::stringstream command;
    command << "./fs_check -c is_file -f " << path_;

    return  invoke(command.str());
}

bool File::isDirectory()
{
    std::stringstream command;
    command << "./fs_check -c is_dir -f " << path_;

    return  invoke(command.str());
}

bool File::hasAccessRights(int accessRights)
{
    std::stringstream command;
    command << "./fs_check -c has_mode -f " << path_ << " -a " << accessRights;

    return  invoke(command.str());
}

bool File::hasSize(size_t size)
{
    std::stringstream command;
    command << "./fs_check -c has_size -f " << path_ << " -a " << size;

    return  invoke(command.str());
}

bool File::hasSubdirectory(std::string const & subdir)
{
    std::stringstream command;
    command << "./fs_check -c has_subdir -f " << path_ << " -a " << subdir;

    return  invoke(command.str());
}

bool File::hasContents(std::string const & contents)
{
    std::stringstream command;
    command << "./fs_check -c has_contents -f " << path_ << " -a " << contents;

    return  invoke(command.str());
}

}