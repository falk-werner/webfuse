#define FUSE_USE_VERSION 31

#include "webfuse/fuse.hpp"
#include <fuse.h>
#include <cstring>
#include <cstdio>

extern "C"
{

static webfuse::filesystem_i * fs_get_filesystem()
{
    struct fuse_context * context = fuse_get_context();
    void * private_data = context->private_data;
    return reinterpret_cast<webfuse::filesystem_i*>(private_data);
}

static uint64_t fs_get_handle(fuse_file_info * info)
{
    return (nullptr != info) ? info->fh : ((uint64_t) -1);
}

static void * fs_init(fuse_conn_info * connection, fuse_config * config)
{
    (void) connection;
    config->use_ino = 1;
    config->entry_timeout = 0;
    config->attr_timeout = 0;
    config->negative_timeout = 0;

    struct fuse_context * context = fuse_get_context();
    return context->private_data;
}


static int fs_access(char const * path, int mode)
{
    auto * const fs = fs_get_filesystem();
    return fs->access(path, mode);
}

static int fs_getattr(char const * path, struct stat * buffer, fuse_file_info * info)
{
    (void) info;

    auto * const fs = fs_get_filesystem();
    return fs->getattr(path, buffer);
}

static int fs_readlink(char const * path, char * buffer, size_t buffer_size)
{
    auto * const fs = fs_get_filesystem();

    std::string out;
    auto result = fs->readlink(path, out);
    if (0 == result)
    {
        snprintf(buffer, buffer_size, "%s", out.c_str());
        result = strlen(buffer);
    }

    return result;
}

static int fs_symlink(char const * target, char const * linkpath)
{
    auto * const fs = fs_get_filesystem();
    return fs->symlink(target, linkpath);
}

static int fs_link(char const * old_path, char const * new_path)
{
    auto * const fs = fs_get_filesystem();
    return fs->link(old_path, new_path);
}

static int fs_rename(char const * from, char const * to, unsigned int flags)
{
    auto * const fs = fs_get_filesystem();
    return fs->rename(from, to, flags);
}

static int fs_chmod(char const * path, mode_t mode, fuse_file_info * info)
{
    (void) info;

    auto * const fs = fs_get_filesystem();
    return fs->chmod(path, mode);
}

static int fs_chown(char const * path, uid_t uid, gid_t gid, fuse_file_info * info)
{
    (void) info;

    auto * const fs = fs_get_filesystem();
    return fs->chown(path, uid, gid);
}

static int fs_truncate(char const * path, off_t size, fuse_file_info * info)
{
    auto * const fs = fs_get_filesystem();
    auto const handle = fs_get_handle(info);

    return fs->truncate(path, size, handle);
}

static int fs_fsync(char const * path, int isdatasync, fuse_file_info * info)
{
    auto * const fs = fs_get_filesystem();
    bool const is_datasync = (is_datasync != 0);
    auto const handle = fs_get_handle(info);

    return fs->fsync(path, is_datasync, handle);
}

static int fs_open(char const * path, fuse_file_info * info)
{
    auto * const fs = fs_get_filesystem();
    auto const flags = info->flags;

    return fs->open(path, flags, info->fh);
}

static int fs_mknod(char const * path, mode_t mode, dev_t raw_rdev)
{
    auto * const fs = fs_get_filesystem();
    auto const rdev = static_cast<uint64_t>(raw_rdev);

    return fs->mknod(path, mode, rdev);
}

static int fs_create(char const * path, mode_t mode, fuse_file_info * info)
{
    auto * const fs = fs_get_filesystem();

    return fs->create(path, mode, info->fh);
}

static int fs_release(char const * path, fuse_file_info * info)
{
    auto * const fs = fs_get_filesystem();
    auto const handle = fs_get_handle(info);

    return fs->release(path, handle);
}

static int fs_unlink(char const * path)
{
    auto * const fs = fs_get_filesystem();
    return fs->unlink(path);
}

static int fs_read(char const * path, char * buffer,
    size_t buffer_size, off_t raw_offset,
    fuse_file_info * info)
{
    auto * const fs = fs_get_filesystem();
    auto const offset = static_cast<uint64_t>(raw_offset);
    auto const handle = fs_get_handle(info);

    return fs->read(path, buffer, buffer_size, offset, handle);
}

static int fs_write(char const * path, char const * buffer,
    size_t buffer_size, off_t raw_offset,
    fuse_file_info * info)
{
    auto * const fs = fs_get_filesystem();
    auto const offset = static_cast<uint64_t>(raw_offset);
    auto const handle = fs_get_handle(info);

    return fs->write(path, buffer, buffer_size, offset, handle);
}

static int fs_mkdir(char const * path, mode_t mode)
{
    auto * const fs = fs_get_filesystem();
    return fs->mkdir(path, mode);
}

static int fs_readdir(char const * path, void * buffer, 
    fuse_fill_dir_t filler, off_t offset, fuse_file_info * info,
    fuse_readdir_flags flags)
{
    auto * const fs = fs_get_filesystem();
    auto handle = fs_get_handle(info);
    std::vector<std::string> names;
    auto const result = fs->readdir(path, names, handle);
    if (0 == result)
    {
        filler(buffer, ".", nullptr, 0, static_cast<fuse_fill_dir_flags>(0));
        filler(buffer, "..", nullptr, 0, static_cast<fuse_fill_dir_flags>(0));
        for (auto const & name: names)
        {
            filler(buffer, name.c_str(), nullptr, 0, static_cast<fuse_fill_dir_flags>(0));
        }
    }

    return result;
}

static int fs_rmdir(char const * path)
{
    auto * const fs = fs_get_filesystem();
    return fs->rmdir(path);
}

static int fs_statfs(char const * path, struct statvfs * buffer)
{
    auto * const fs = fs_get_filesystem();
    return fs->statfs(path, buffer);
}

}

namespace webfuse
{

class fuse::detail
{
public:
    filesystem_i & filesystem;
};

fuse::fuse(filesystem_i & filesystem)
: d(new detail{filesystem})
{

}

fuse::~fuse()
{
    delete d;
}

fuse::fuse(fuse && other)
{
    this->d = other.d;
    other.d = nullptr;
}

fuse& fuse::operator=(fuse && other)
{
    if (this != &other)
    {
        delete d;
        this->d = other.d;
        other.d = nullptr;
    }

    return *this;
}

int fuse::run(int argc, char * argv[])
{
    void * context = reinterpret_cast<void*>(&d->filesystem);
    struct fuse_operations operations;
    memset(reinterpret_cast<void*>(&operations), 0, sizeof(operations));
    operations.init = fs_init;
    operations.access = fs_access;
    operations.getattr = fs_getattr;
    operations.readlink = fs_readlink;
    operations.symlink = fs_symlink;
    operations.link = fs_link;
    operations.rename = fs_rename;
    operations.chmod = fs_chmod;
    operations.chown = fs_chown;
    operations.truncate = fs_truncate;
    operations.fsync = fs_fsync;
    operations.open = fs_open;
    operations.mknod = fs_mknod;
    operations.create = fs_create;
    operations.release = fs_release;
    operations.unlink = fs_unlink;
    operations.read = fs_read;
    operations.write = fs_write;
    operations.mkdir = fs_mkdir;
    operations.readdir = fs_readdir;
    operations.rmdir = fs_rmdir;
    operations.statfs = fs_statfs;

    return fuse_main(argc, argv, &operations, context);
}

}