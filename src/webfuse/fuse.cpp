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

static webfuse::filehandle fs_get_handle(fuse_file_info * info)
{
    return (nullptr != info) ? info->fh : webfuse::invalid_handle;
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


static int fs_access(char const * path, int raw_mode)
{
    auto * const fs = fs_get_filesystem();
    auto const mode = webfuse::access_mode::from_int(raw_mode);
    auto const result = fs->access(path, mode);

    return result.to_fusestatus();
}

static int fs_getattr(char const * path, struct stat * buffer, fuse_file_info * info)
{
    auto * const fs = fs_get_filesystem();
    webfuse::file_attributes attributes(*buffer);

    auto const result = fs->getattr(path, attributes);
    attributes.to_stat(*buffer);

    return result.to_fusestatus();
}

static int fs_readlink(char const * path, char * buffer, size_t buffer_size)
{
    auto * const fs = fs_get_filesystem();

    std::string out;
    auto result = fs->readlink(path, out);
    if (webfuse::status::good == result)
    {
        snprintf(buffer, buffer_size, "%s", out.c_str());
        result = strlen(buffer);
    }

    return result.to_fusestatus();
}

static int fs_symlink(char const * target, char const * linkpath)
{
    auto * const fs = fs_get_filesystem();
    auto const result = fs->symlink(target, linkpath);
    return result.to_fusestatus();
}

static int fs_link(char const * old_path, char const * new_path)
{
    auto * const fs = fs_get_filesystem();
    auto const result = fs->link(old_path, new_path);
    return result.to_fusestatus();
}

static int fs_rename(char const * from, char const * to, unsigned int flags)
{
    // ToDo: provide flags 
    auto * const fs = fs_get_filesystem();
    auto const result = fs->rename(from, to);
    return result.to_fusestatus();
}

static int fs_chmod(char const * path, mode_t raw_mode, fuse_file_info * info)
{
    auto * const fs = fs_get_filesystem();
    auto const mode = webfuse::filemode::from_mode(raw_mode);
    auto const result = fs->chmod(path, mode);
    return result.to_fusestatus();
}

static int fs_chown(char const * path, uid_t raw_uid, 
    gid_t raw_gid, fuse_file_info * info)
{
    auto * const fs = fs_get_filesystem();
    auto const uid = webfuse::user_id::from_uid(raw_uid);
    auto const gid = webfuse::group_id::from_gid(raw_gid);
    auto const result = fs->chown(path, uid, gid);
    return result.to_fusestatus();
}

static int fs_truncate(char const * path, off_t raw_size, fuse_file_info * info)
{
    auto * const fs = fs_get_filesystem();
    auto const size = static_cast<uint64_t>(raw_size);
    auto const handle = fs_get_handle(info);

    auto const result = fs->truncate(path, size, handle);
    return result.to_fusestatus();
}

static int fs_fsync(char const * path, int isdatasync, fuse_file_info * info)
{
    auto * const fs = fs_get_filesystem();
    bool const is_datasync = (is_datasync != 0);
    auto const handle = fs_get_handle(info);

    auto const result = fs->fsync(path, is_datasync, handle);
    return result.to_fusestatus();
}

static int fs_open(char const * path, fuse_file_info * info)
{
    auto * const fs = fs_get_filesystem();
    auto const flags = webfuse::openflags::from_int(info->flags);

    auto const result = fs->open(path, flags, info->fh);
    return result.to_fusestatus();
}

static int fs_mknod(char const * path, mode_t raw_mode, dev_t raw_rdev)
{
    auto * const fs = fs_get_filesystem();
    auto const mode = webfuse::filemode::from_mode(raw_mode);
    auto const rdev = static_cast<uint64_t>(raw_rdev);

    auto const result = fs->mknod(path, mode, rdev);
    return result.to_fusestatus();
}

static int fs_create(char const * path, mode_t raw_mode, fuse_file_info * info)
{
    auto * const fs = fs_get_filesystem();
    auto const mode = webfuse::filemode::from_mode(raw_mode);

    auto const result = fs->create(path, mode, info->fh);
    return result.to_fusestatus();
}

static int fs_release(char const * path, fuse_file_info * info)
{
    auto * const fs = fs_get_filesystem();
    auto const handle = fs_get_handle(info);

    auto const result = fs->release(path, handle);
    return result.to_fusestatus();
}

static int fs_unlink(char const * path)
{
    auto * const fs = fs_get_filesystem();
    auto const result = fs->unlink(path);
    return result.to_fusestatus();
}

static int fs_read(char const * path, char * buffer,
    size_t buffer_size, off_t raw_offset,
    fuse_file_info * info)
{
    auto * const fs = fs_get_filesystem();
    auto const offset = static_cast<uint64_t>(raw_offset);
    auto const handle = fs_get_handle(info);

    auto const result = fs->read(path, buffer, buffer_size, offset, handle);
    return result.to_fusestatus();
}

static int fs_write(char const * path, char const * buffer,
    size_t buffer_size, off_t raw_offset,
    fuse_file_info * info)
{
    auto * const fs = fs_get_filesystem();
    auto const offset = static_cast<uint64_t>(raw_offset);
    auto const handle = fs_get_handle(info);

    auto const result = fs->write(path, buffer, buffer_size, offset, handle);
    return result.to_fusestatus();
}

static int fs_mkdir(char const * path, mode_t raw_mode)
{
    auto * const fs = fs_get_filesystem();
    auto const mode = webfuse::filemode::from_mode(raw_mode);

    auto const result = fs->mkdir(path, mode);
    return result.to_fusestatus();
}

static int fs_readdir(char const * path, void * buffer, 
    fuse_fill_dir_t filler, off_t offset, fuse_file_info * info,
    fuse_readdir_flags flags)
{
    auto * const fs = fs_get_filesystem();
    auto handle = fs_get_handle(info);
    std::vector<std::string> names;
    auto const result = fs->readdir(path, names, handle);
    if (result.is_good())
    {
        filler(buffer, ".", nullptr, 0, static_cast<fuse_fill_dir_flags>(0));
        filler(buffer, "..", nullptr, 0, static_cast<fuse_fill_dir_flags>(0));
        for (auto const & name: names)
        {
            filler(buffer, name.c_str(), nullptr, 0, static_cast<fuse_fill_dir_flags>(0));
        }
    }

    return result.to_fusestatus();
}

static int fs_rmdir(char const * path)
{
    auto * const fs = fs_get_filesystem();
    auto const result = fs->rmdir(path);
    return result.to_fusestatus();
}

static int fs_statfs(char const * path, struct statvfs * buffer)
{
    auto * const fs = fs_get_filesystem();
    webfuse::filesystem_statistics statistics(*buffer);

    auto const result = fs->statfs(path, statistics);
    statistics.copy_to(*buffer);

    return result.to_fusestatus();

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