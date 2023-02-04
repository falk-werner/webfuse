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
        return ERRNO.ENOENT;
    }

    chown(path, uid, gid) {
        return ERRNO.ENOENT;
    }

    truncate(path, size, fd) {
        return ERRNO.ENOENT;
    }

    open(path, flags) {
        return [ERRNO.ENOENT, 0];
    }

    mknod(path, mode, rdev) {
        return ERRNO.ENOENT;
    }

    create(path, mode) {
        return [ERNNO.ENOEND, 0];
    }

    release(path, fd) {
        return ERRNO.ENOENT;
    }

    unlink(path) {
        return ERRNO.ENOENT;
    }

    read(path, size, offset, fd) {
        return ERRNO.ENOENT;
    }

    write(path, data, offset, fd) {
        return ERRNO.ENOENT;
    }

    mkdir(path, mode) {
        return ERRNO.ENOENT;
    }

    readdir(path) {
        return ERRNO.ENOENT;
    }

    rmdir(path)  {
        return ERRNO.ENOENT;
    }

    statfs(path) {
        return ERRNO.ENOENT;
    }

    getcreds() {
        return "";
    }
}

export { BaseFileSystem }
