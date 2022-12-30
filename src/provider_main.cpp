#include "webfuse/provider.hpp"

#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#include <csignal>
#include <iostream>

namespace
{

static bool shutdown_requested = false;

void on_signal(int _)
{
    (void) _;
    shutdown_requested = true;
}

class filesystem: public webfuse::filesystem_i
{
public:
    explicit filesystem(std::string const & base_path)
    : base_path_(base_path)
    {

    }

    ~filesystem() override
    {

    }

    int access(std::string const & path, int mode) override
    {
        auto const full_path = get_full_path(path);
        std::cout << "access: " << full_path << std::endl;

        auto const result = ::access(full_path.c_str(), mode);
        return (result == 0) ? 0 : -errno;
    }

    int getattr(std::string const & path, struct stat * attr) override
    {
        auto const full_path = get_full_path(path);
        std::cout << "getattr: " << full_path << std::endl;

        auto const result = lstat(full_path.c_str(), attr);
        return (result == 0) ? 0 : -errno;
    }

    int readlink(std::string const & path, std::string & out) override
    {
        return -ENOENT;
    }
    
    int symlink(std::string const & target, std::string const & linkpath) override
    {
        return -ENOENT;
    }

    int link(std::string const & old_path, std::string const & new_path) override
    {
        return -ENOENT;
    }

    int rename(std::string const & old_path, std::string const & new_path, int flags) override
    {
        return -ENOENT;
    }

    int chmod(std::string const & path, mode_t mode) override
    {
        return -ENOENT;
    }

    int chown(std::string const & path, uid_t uid, gid_t gid) override
    {
        return -ENOENT;
    }

    int truncate(std::string const & path, uint64_t size, uint64_t handle) override
    {
        return -ENOENT;
    }

    int fsync(std::string const & path, bool is_datasync, uint64_t handle) override
    {
        return -ENOENT;
    }

    int utimens(std::string const &path, struct timespec tv[2], uint64_t handle) override
    {
        return -ENOENT;
    }

    int open(std::string const & path, int flags, uint64_t & handle) override
    {
        return -ENOENT;
    }

    int mknod(std::string const & path, mode_t mode, dev_t rdev) override
    {
        return -ENOENT;
    }

    int create(std::string const & path, mode_t mode, uint64_t & handle) override
    {
        return -ENOENT;
    }

    int release(std::string const & path, uint64_t handle) override
    {
        return -ENOENT;
    }

    int unlink(std::string const & path) override
    {
        return -ENOENT;
    }

    int read(std::string const & path, char * buffer, size_t buffer_size, uint64_t offset, uint64_t handle) override
    {
        return -ENOENT;
    }

    int write(std::string const & path, char const * buffer, size_t buffer_size, uint64_t offset, uint64_t handle) override
    {
        return -ENOENT;
    }

    int mkdir(std::string const & path, mode_t mode) override
    {
        return -ENOENT;
    }

    int readdir(std::string const & path, std::vector<std::string> & entries, uint64_t handle) override
    {
        auto const full_path = get_full_path(path);
        std::cout << "readdir: " << full_path << std::endl;

        int result = 0;
        DIR * directory = opendir(full_path.c_str());
        if (NULL != directory)
        {
            dirent * entry = ::readdir(directory);
            while (entry != nullptr)
            {
                entries.push_back(std::string(entry->d_name));
                entry = ::readdir(directory);
            }
            closedir(directory);
        }
        else
        {
            result = -errno;
        }

        return result;
    }

    int rmdir(std::string const & path) override
    {
        return -ENOENT;
    }

    int statfs(std::string const & path, struct statvfs * statistics) override
    {
        return -ENOENT;
    }


private:
    std::string get_full_path(std::string const & path)
    {
        return base_path_ + path;
    }

    std::string base_path_;
};

}



int main(int argc, char* argv[])
{
    signal(SIGINT, &on_signal);
    signal(SIGTERM, &on_signal);

    filesystem fs(".");
    webfuse::provider provider(fs);
    provider.set_connection_listener([](bool connected) {
        if (!connected)
        {
            shutdown_requested = true;
        }
    });
    provider.connect("ws://localhost:8080/");
    while (!shutdown_requested)
    {
        provider.service();
    }
    return EXIT_SUCCESS;
}