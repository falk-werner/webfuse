#include "webfuse/provider.hpp"
#include "webfuse/version.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>

#include <getopt.h>
#include <csignal>
#include <iostream>

namespace
{

enum class command
{
    run,
    show_help,
    show_version
};

class context
{
public:
    context(int argc, char* argv[])
    : base_path(".")
    , url("")
    , cmd(command::run)
    , exit_code()
    {
        struct option const long_options[] =
        {
            {"path"   , required_argument, nullptr, 'p'},
            {"url"    , required_argument, nullptr, 'u'},
            {"version", no_argument      , nullptr, 'v'},
            {"help"   , no_argument      , nullptr, 'h'},
            {nullptr  , 0                , nullptr, 0  }
        };

        optind = 0;
        opterr = 0;
        bool finished = false;
        while (!finished)
        {
            int option_index = 0;
            const int c = getopt_long(argc, argv, "p:u:vh", long_options, &option_index);
            switch (c)
            {
                case -1:
                    finished = true;
                    break;
                case 'p':
                    base_path = optarg;
                    break;
                case 'u':
                    url = optarg;
                    break;
                case 'h':
                    cmd = command::show_help;
                    break;
                case 'v':
                    cmd = command::show_version;
                    break;                
                default:
                    std::cerr << "error: unknown option" << std::endl;
                    cmd = command::show_help;
                    exit_code = EXIT_FAILURE;
                    finished = true;
                    break;
            }
        }

        if ((cmd == command::run) && (url.empty()))
        {
            std::cerr << "error: missing url" << std::endl;
            cmd = command::show_help;
            exit_code = EXIT_FAILURE;
        }
    }

    std::string base_path;
    std::string url;
    command cmd;
    int exit_code;
};

void print_usage()
{
    std::cout << R"(webfuse2 provider, (c) 2022 by Falk Werner
expose a local directory via webfuse2

Usage:
    webfuse-provider -u <url> [-p <path>]

Options:
    --url, -u       set url of webfuse2 service
    --path, -p      set path of directory to expose (default: .)
    --version, -v   print version and quit
    --help, -h      print this message and quit

Examples:
    webfuse-provider -u ws://localhost:8080/
    webfuse-provider -u ws://localhost:8080/ -p /some/directory
)";
}

void print_version()
{
    std::cout << webfuse::get_version() << std::endl;
}

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
    {
        char buffer[PATH_MAX];
        char * resolved_path = ::realpath(base_path.c_str(), buffer);
        if (nullptr == resolved_path)
        {
            throw std::runtime_error("failed to resolve path");
        }

        struct stat info;
        int const rc = stat(resolved_path, &info);
        if (!S_ISDIR(info.st_mode))
        {
            throw std::runtime_error("path is not a directory");
        }

        base_path_ = resolved_path;
    }

    ~filesystem() override
    {

    }

    int access(std::string const & path, int mode) override
    {
        auto const full_path = get_full_path(path);

        auto const result = ::access(full_path.c_str(), mode);
        return (result == 0) ? 0 : -errno;
    }

    int getattr(std::string const & path, struct stat * attr) override
    {
        auto const full_path = get_full_path(path);

        auto const result = lstat(full_path.c_str(), attr);
        return (result == 0) ? 0 : -errno;
    }

    int readlink(std::string const & path, std::string & out) override
    {
        auto const full_path = get_full_path(path);

        char buffer[PATH_MAX];
        int const result = ::readlink(full_path.c_str(), buffer, PATH_MAX);
        if ((0 <= result) && (result < PATH_MAX))
        {
            buffer[result] = '\0';
            out = buffer;
        }

        return (result >= 0) ? 0 : -errno;
    }
    
    int symlink(std::string const & from, std::string const & to) override
    {
        auto const full_from = ('/' == from.at(0)) ? get_full_path(from) : from;
        auto const full_to = get_full_path(to);

        int const result = ::symlink(full_from.c_str(), full_to.c_str());
        return (result == 0) ? 0 : -errno;
    }

    int link(std::string const & old_path, std::string const & new_path) override
    {
        auto const from = get_full_path(old_path);
        auto const to = get_full_path(new_path);

        int const result = ::link(from.c_str(), to.c_str());
        return (result == 0) ? 0 : -errno;
    }

    int rename(std::string const & old_path, std::string const & new_path, int flags) override
    {
        auto const full_old = get_full_path(old_path);
        auto const full_new = get_full_path(new_path);

        int const result = ::renameat2(-1, full_old.c_str(), -1, full_new.c_str(), flags);
        return (result == 0) ? 0 : -errno;
    }

    int chmod(std::string const & path, mode_t mode) override
    {
        auto const full_path = get_full_path(path);

        int const result = ::chmod(full_path.c_str(), mode);
        return (result == 0) ? 0 : -errno;
    }

    int chown(std::string const & path, uid_t uid, gid_t gid) override
    {
        auto const full_path = get_full_path(path);

        int const result = ::chown(full_path.c_str(), uid, gid);
        return (result == 0) ? 0 : -errno;
    }

    int truncate(std::string const & path, uint64_t size, uint64_t handle) override
    {
        auto const full_path = get_full_path(path);

        int result = 0;
        if (handle == webfuse::invalid_handle)
        {
            result = ::truncate(full_path.c_str(), size);
        }
        else
        {
            result = ::ftruncate(static_cast<int>(handle), size);
        }

        return (result == 0) ? 0 : -errno;
    }

    int fsync(std::string const & path, bool is_datasync, uint64_t handle) override
    {
        int result = 0;
        if (handle != webfuse::invalid_handle)
        {
            if (!is_datasync)
            {
                result = ::fsync(static_cast<int>(handle));
            }
            else
            {
                result = ::fdatasync(static_cast<int>(handle));
            }
        }
        // we do not sync files, which are not open

        return (result == 0) ? 0 : -errno;
    }

    int utimens(std::string const &path, struct timespec const tv[2], uint64_t handle) override
    {
        int result = 0;
        if (handle == webfuse::invalid_handle)
        {
            auto const full_path = get_full_path(path);
            result = ::utimensat(-1, full_path.c_str(), tv, 0);
        }
        else
        {
            result = ::futimens(static_cast<int>(handle), tv);
        }

        return (result == 0) ? 0 : -errno;
    }

    int open(std::string const & path, int flags, uint64_t & handle) override
    {
        auto const full_path = get_full_path(path);
        int const fd = ::open(full_path.c_str(), flags);
        if (0 <= fd)
        {
            handle = static_cast<int>(fd);
        }
        
        return (0 <= fd) ? 0 : -errno;
    }

    int mknod(std::string const & path, mode_t mode, dev_t rdev) override
    {
        auto const full_path = get_full_path(path);
        int const result = ::mknod(full_path.c_str(), mode, rdev);

        return (result == 0) ? 0 : -errno;
    }

    int create(std::string const & path, mode_t mode, uint64_t & handle) override
    {
        auto const full_path = get_full_path(path);
        int const fd = ::creat(full_path.c_str(), mode);
        if (0 <= fd)
        {
            handle = static_cast<int>(fd);
        }
        
        return (0 <= fd) ? 0 : -errno;
    }

    int release(std::string const & path, uint64_t handle) override
    {
        int result = 0;
        if (handle != webfuse::invalid_handle)
        {
            result = ::close(static_cast<int>(handle));
        }

        return (result == 0) ? 0 : -errno;
    }

    int unlink(std::string const & path) override
    {
        auto const full_path = get_full_path(path);
        int const result = ::unlink(full_path.c_str());

        return (result == 0) ? 0 : -errno;
    }

    int read(std::string const & path, char * buffer, size_t buffer_size, uint64_t offset, uint64_t handle) override
    {
        int result = -1;
        if (handle != webfuse::invalid_handle)
        {
            auto const full_path = get_full_path(path);
            int fd = ::open(full_path.c_str(), O_RDONLY);
            if (0 <= fd)
            {
                result = ::pread(fd, buffer, buffer_size, offset);
                ::close(fd);
            }
        }
        else
        {
            result = ::pread(static_cast<int>(handle), buffer, buffer_size, offset);
        }

        return (result >= 0) ? result : -errno;
    }

    int write(std::string const & path, char const * buffer, size_t buffer_size, uint64_t offset, uint64_t handle) override
    {
        int result = -1;
        if (handle == webfuse::invalid_handle)
        {
            auto const full_path = get_full_path(path);
            int fd = ::open(full_path.c_str(), O_WRONLY);
            if (0 <= fd)
            {
                result = ::pwrite(fd, buffer, buffer_size, offset);
                ::close(fd);
            }
        }
        else
        {
            result = ::pwrite(static_cast<int>(handle), buffer, buffer_size, offset);

        }

        return (result >= 0) ? result : -errno;
    }

    int mkdir(std::string const & path, mode_t mode) override
    {
        auto const full_path = get_full_path(path);
        int const result = ::mkdir(full_path.c_str(), mode);

        return (result == 0) ? 0 : -errno;
    }

    int readdir(std::string const & path, std::vector<std::string> & entries) override
    {
        auto const full_path = get_full_path(path);

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
        auto const full_path = get_full_path(path);
        int const result = ::rmdir(full_path.c_str());

        return (result == 0) ? 0 : -errno;
    }

    int statfs(std::string const & path, struct statvfs * statistics) override
    {
        auto const full_path = get_full_path(path);
        int const result = ::statvfs(full_path.c_str(), statistics);

        return (result == 0) ? 0 : -errno;
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
    context ctx(argc, argv);

    switch (ctx.cmd)
    {
        case command::run:
            try
            {
                signal(SIGINT, &on_signal);
                signal(SIGTERM, &on_signal);

                filesystem fs(ctx.base_path);
                webfuse::provider provider(fs);
                provider.set_connection_listener([](bool connected) {
                    if (!connected)
                    {
                        shutdown_requested = true;
                    }
                });
                provider.connect(ctx.url);
                while (!shutdown_requested)
                {
                    provider.service();
                }
            }
            catch (std::exception const & ex)
            {
                std::cerr << "error: " << ex.what() << std::endl;
                ctx.exit_code = EXIT_FAILURE;
            }
            catch (...)
            {
                std::cerr << "error: unspecified error" << std::endl;
                ctx.exit_code = EXIT_FAILURE;
            }
            break;
        case command::show_version:
            print_version();
            break;
        case command::show_help:
            // fall-through
        default:
            print_usage();
            break;
    }

    return ctx.exit_code;
}