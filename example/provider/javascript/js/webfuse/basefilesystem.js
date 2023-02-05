import { ERRNO } from "./errno.js"

class BaseFileSystem {

    access(path, mode) {
        return ERRNO.ENOENT;
    }

    getattr(path) {
        return ERRNO.ENOENT;
    }

    readlink(path) {
        return ERRNO.ENOENT;
    }

    symlink(target, linkpath) {
        return ERRNO.ENOENT;
    }

    link(oldpath, newpath) {
        return ERRNO.ENOENT;
    }

    rename(oldpath, newpath, flags) {
        return ERRNO.ENOENT;
    }

    chmod(path, mode) {
        return ERRNO.EPERM;
    }

    chown(path, uid, gid) {
        return ERRNO.EPERM;
    }

    truncate(path, size, fd) {
        return ERRNO.EPERM;
    }

    fsync(path, isDataSync, fd) {
        return 0;
    }

    open(path, flags) {
        return [ERRNO.ENOENT, 0];
    }

    mknod(path, mode, rdev) {
        return ERRNO.EPERM;
    }

    create(path, mode) {
        return [ERRNO.EPERM, 0];
    }

    release(path, fd) {
        return 0;
    }

    unlink(path) {
        return ERRNO.EPERM;
    }

    read(path, size, offset, fd) {
        return ERRNO.EBADF;
    }

    write(path, data, offset, fd) {
        return ERRNO.EBADF;
    }

    mkdir(path, mode) {
        return ERRNO.EPERM;
    }

    readdir(path) {
        return ERRNO.ENOENT;
    }

    rmdir(path)  {
        return ERRNO.EPERM;
    }

    statfs(path) {
        return ERRNO.ENOSYS;
    }

    utimens(path, atime, mtime) {
        return ERRNO.ENOSYS;
    }

    getcreds() {
        return "";
    }

    connectionstatechanged(state) {
        // pass
    }
}

export { BaseFileSystem }
