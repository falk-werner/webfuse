#include "webfuse/provider.hpp"
#include "webfuse/version.hpp"

#include <unistd.h>
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

    int utimens(std::string const &path, struct timespec const tv[2], uint64_t handle) override
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

    int readdir(std::string const & path, std::vector<std::string> & entries) override
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
    context ctx(argc, argv);

    switch (ctx.cmd)
    {
        case command::run:
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