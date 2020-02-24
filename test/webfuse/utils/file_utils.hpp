#ifndef WF_TEST_FILE_UTILS_HPP
#define WF_TEST_FILE_UTILS_HPP

#include <string>

namespace webfuse_test
{

bool is_dir(std::string const & path);

bool is_symlink(std::string const & path);

bool is_same_path(std::string const & path, std::string const & other);

}

#endif
