#!/usr/bin/env python3

"""Example webfuse provider written in python."""

import asyncio
import os
import stat
import errno
import getpass
import argparse
import websockets

INVALID_FD = 0xffffffffffffffff

F_OK = 0
R_OK = 4
W_OK = 2
X_OK = 1

RESPONSE = 0x80

ERRNO = {
    -errno.E2BIG        :   -7,
    -errno.EACCES       :  -13,
    -errno.EAGAIN       :  -11,
    -errno.EBADF        :   -9,
    -errno.EBUSY        :  -16,
    -errno.EDESTADDRREQ :  -89,
    -errno.EDQUOT       : -122,
    -errno.EEXIST       :  -17,
    -errno.EFAULT       :  -14,
    -errno.EFBIG        :  -27,
    -errno.EINTR        :   -4,
    -errno.EINVAL       :  -22,
    -errno.EIO          :   -5,
    -errno.EISDIR       :  -21,
    -errno.ELOOP        :  -40,
    -errno.EMFILE       :  -24,
    -errno.EMLINK       :  -31,
    -errno.ENAMETOOLONG :  -36,
    -errno.ENFILE       :  -23,
    -errno.ENODATA      :  -61,
    -errno.ENODEV       :  -19,
    -errno.ENOENT       :   -2,
    -errno.ENOMEM       :  -12,
    -errno.ENOSPC       :  -28,
    -errno.ENOSYS       :  -38,
    -errno.ENOTDIR      :  -20,
    -errno.ENOTEMPTY    :  -39,
    -errno.ENOTSUP      :  -95,
    -errno.ENXIO        :   -6,
    -errno.EOVERFLOW    :  -75,
    -errno.EPERM        :   -1,
    -errno.EPIPE        :  -32,
    -errno.ERANGE       :  -34,
    -errno.EROFS        :  -30,
    -errno.ETXTBSY      :  -26,
    -errno.EXDEV        :  -18
}

RENAME_NOREPLACE = 0x01
RENAME_EXCHANGE  = 0x02

MODE_REG  = 0o100000
MODE_DIR  = 0o040000
MODE_CHR  = 0o020000
MODE_BLK  = 0o060000
MODE_FIFO = 0o010000
MODE_LNK  = 0o120000
MODE_SOCK = 0o140000

O_RDONLY = 0o00
O_WRONLY = 0o01
O_RDWR   = 0o02

O_APPEND    = 0o00002000
O_ASYNC     = 0o00020000
O_CLOEXEC   = 0o02000000
O_CREAT     = 0o00000100
O_DIRECT    = 0o00040000
O_DIRECTORY = 0o00200000
O_DSYNC     = 0o00010000
O_EXCL      = 0o00000200
O_LARGEFILE = 0o00100000
O_NOATIME   = 0o01000000
O_NOCTTY    = 0o00000400
O_NOFOLLOW  = 0o00400000
O_NONBLOCK  = 0o00004000
O_NDELAY    = 0o00004000
O_PATH      = 0o10000000
O_SYNC      = 0o04010000
O_TMPFILE   = 0o20200000
O_TRUNC     = 0o00001000

class MessageReader:
    """Reads webfuse messages from buffer."""

    def __init__(self, buffer):
        self.buffer = buffer
        self.offset = 0

    def read_u8(self):
        value = self.buffer[self.offset]
        self.offset += 1
        return value

    def read_bool(self):
        return self.read_u8() == 1

    def read_u32(self):
        value = (self.buffer[self.offset] << 24) + (self.buffer[self.offset + 1] << 16) + \
            (self.buffer[self.offset + 2] << 8) + self.buffer[self.offset + 3]
        self.offset += 4
        return value

    def read_u64(self):
        value = (
            (self.buffer[self.offset    ] << 56) +
            (self.buffer[self.offset + 1] << 48) +
            (self.buffer[self.offset + 2] << 40) +
            (self.buffer[self.offset + 3] << 32) +
            (self.buffer[self.offset + 4] << 24) +
            (self.buffer[self.offset + 5] << 16) +
            (self.buffer[self.offset + 6] <<  8) +
             self.buffer[self.offset + 7])
        self.offset += 8
        return value


    def read_str(self):
        return self.read_bytes().decode()

    def read_bytes(self):
        size = self.read_u32()
        value = self.buffer[self.offset : self.offset + size]
        self.offset += size
        return value

    def read_path(self, base_path):
        local_path = self.read_str().lstrip('/')
        return os.path.join(base_path, local_path)

    def read_access_mode(self):
        value = self.read_u8()
        mode  = os.F_OK if F_OK == (value & F_OK) else 0
        mode |= os.R_OK if R_OK == (value & R_OK) else 0
        mode |= os.W_OK if W_OK == (value & W_OK) else 0
        mode |= os.X_OK if X_OK == (value & X_OK) else 0
        return mode

    def read_rename_flags(self):
        return self.read_u8()

    def read_mode(self):
        value = self.read_u32()
        mode = value & 0o7777
        mode |= stat.S_IFREG  if MODE_REG  == (value & MODE_REG ) else 0
        mode |= stat.S_IFDIR  if MODE_DIR  == (value & MODE_DIR ) else 0
        mode |= stat.S_IFCHR  if MODE_CHR  == (value & MODE_CHR ) else 0
        mode |= stat.S_IFBLK  if MODE_BLK  == (value & MODE_BLK ) else 0
        mode |= stat.S_IFIFO  if MODE_FIFO == (value & MODE_FIFO) else 0
        mode |= stat.S_IFLNK  if MODE_LNK  == (value & MODE_LNK ) else 0
        mode |= stat.S_IFSOCK if MODE_SOCK == (value & MODE_SOCK) else 0
        return mode

    def read_openflags(self):
        value = self.read_u32()
        flags = 0
        # Access Mode
        flags |= os.O_RDONLY if O_RDONLY == (value & O_RDONLY) else 0
        flags |= os.O_WRONLY if O_WRONLY == (value & O_WRONLY) else 0
        flags |= os.O_RDWR   if O_RDWR   == (value & O_RDWR  ) else 0
        # Flags
        flags |= os.O_APPEND    if O_APPEND    == (value & O_APPEND   ) else 0
        flags |= os.O_ASYNC     if O_ASYNC     == (value & O_ASYNC    ) else 0
        flags |= os.O_CLOEXEC   if O_CLOEXEC   == (value & O_CLOEXEC  ) else 0
        flags |= os.O_CREAT     if O_CREAT     == (value & O_CREAT    ) else 0
        flags |= os.O_DIRECT    if O_DIRECT    == (value & O_DIRECT   ) else 0
        flags |= os.O_DIRECTORY if O_DIRECTORY == (value & O_DIRECTORY) else 0
        flags |= os.O_DSYNC     if O_DSYNC     == (value & O_DSYNC    ) else 0
        flags |= os.O_EXCL      if O_EXCL      == (value & O_EXCL     ) else 0
        flags |= os.O_LARGEFILE if O_LARGEFILE == (value & O_LARGEFILE) else 0
        flags |= os.O_NOCTTY    if O_NOCTTY    == (value & O_NOCTTY   ) else 0
        flags |= os.O_NOFOLLOW  if O_NOFOLLOW  == (value & O_NOFOLLOW ) else 0
        flags |= os.O_NONBLOCK  if O_NONBLOCK  == (value & O_NONBLOCK ) else 0
        flags |= os.O_NDELAY    if O_NDELAY    == (value & O_NDELAY   ) else 0
        flags |= os.O_PATH      if O_PATH      == (value & O_PATH     ) else 0
        flags |= os.O_SYNC      if O_SYNC      == (value & O_SYNC     ) else 0
        flags |= os.O_TMPFILE   if O_TMPFILE   == (value & O_TMPFILE  ) else 0
        flags |= os.O_TRUNC     if O_TRUNC     == (value & O_TRUNC    ) else 0
        return flags



class MessageWriter:
    """"Writes webfuse messages into buffer."""

    def __init__(self, message_id, message_type):
        self.buffer = []
        self.write_u32(message_id)
        self.write_u8(message_type)

    def write_u8(self, value):
        self.buffer.append(value)

    def write_u32(self, value):
        self.buffer.extend([
            (value >> 24) & 0xff,
            (value >> 16) & 0xff,
            (value >>  8) & 0xff,
             value        & 0xff
        ])

    def write_u64(self, value):
        self.buffer.extend([
            (value >> 56) & 0xff,
            (value >> 48) & 0xff,
            (value >> 40) & 0xff,
            (value >> 32) & 0xff,
            (value >> 24) & 0xff,
            (value >> 16) & 0xff,
            (value >>  8) & 0xff,
             value        & 0xff
        ])

    def write_i32(self, value):
        self.write_u32(value & 0xffffffff)

    def write_result(self, value):
        if 0 > value:
            value = ERRNO.get(value, value)
        self.write_i32(value)

    def write_str(self, value):
        data = value.encode('utf-8')
        self.write_bytes(data)

    def write_bytes(self, value):
        size = len(value)
        self.write_u32(size)
        self.buffer.extend(value)

    def write_strings(self, values):
        count = len(values)
        self.write_u32(count)
        for value in values:
            self.write_str(value)

    def get_bytes(self):
        return bytearray(self.buffer)

# pylint: disable=too-many-public-methods
class FilesystemProvider:
    """Webfuse filesystem provider."""

    def __init__(self, path, url, token):
        self.root = os.path.abspath(path)
        self.url = url
        self.token = token
        self.commands = {
            0x01: FilesystemProvider.access,
            0x02: FilesystemProvider.getattr,
            0x03: FilesystemProvider.readlink,
            0x04: FilesystemProvider.symlink,
            0x05: FilesystemProvider.link,
            0x06: FilesystemProvider.rename,
            0x07: FilesystemProvider.chmod,
            0x08: FilesystemProvider.chown,
            0x09: FilesystemProvider.truncate,
            0x0a: FilesystemProvider.fsync,
            0x0b: FilesystemProvider.open,
            0x0c: FilesystemProvider.mknod,
            0x0d: FilesystemProvider.create,
            0x0e: FilesystemProvider.release,
            0x0f: FilesystemProvider.unlink,
            0x10: FilesystemProvider.read,
            0x11: FilesystemProvider.write,
            0x12: FilesystemProvider.mkdir,
            0x13: FilesystemProvider.readdir,
            0x14: FilesystemProvider.rmdir,
            0x15: FilesystemProvider.statfs,
            0x16: FilesystemProvider.utimens,
            0x17: FilesystemProvider.getcreds,
        }

    async def run(self):
        extra_headers = [("X-Auth-Token", self.token)] if self.token != "" else []
        # pylint: disable-next=no-member
        async with websockets.connect(self.url, extra_headers=extra_headers) as connection:
            while True:
                request = await connection.recv()
                reader = MessageReader(request)
                message_id = reader.read_u32()
                message_type = reader.read_u8()
                writer = MessageWriter(message_id, RESPONSE + message_type)
                if message_type in self.commands:
                    method = self.commands[message_type]
                    method(self, reader, writer)
                else:
                    print(f"unknown message type: {message_type}")
                response = writer.get_bytes()
                await connection.send(response)

    def access(self, reader, writer):
        path = reader.read_path(self.root)
        mode = reader.read_access_mode()
        result = -errno.EACCES
        try:
            if os.access(path, mode) is True:
                result = 0
        except OSError as ex:
            result = -ex.errno
        writer.write_result(result)

    def getattr(self, reader, writer):
        path = reader.read_path(self.root)
        try:
            attr = os.lstat(path)
        except OSError as ex:
            writer.write_result(-ex.errno)
            return
        writer.write_result(0)
        writer.write_u64(attr.st_ino)
        writer.write_u64(attr.st_nlink)
        writer.write_u32(attr.st_mode)
        writer.write_i32(attr.st_uid)
        writer.write_i32(attr.st_gid)
        writer.write_u64(attr.st_dev)
        writer.write_u64(attr.st_size)
        writer.write_u64(attr.st_blocks)
        writer.write_u64(int(attr.st_atime))
        writer.write_u32(attr.st_atime_ns)
        writer.write_u64(int(attr.st_mtime))
        writer.write_u32(attr.st_mtime_ns)
        writer.write_u64(int(attr.st_ctime))
        writer.write_u32(attr.st_ctime_ns)

    def readlink(self, reader, writer):
        path = reader.read_path(self.root)
        try:
            link = os.readlink(path)
        except OSError as ex:
            writer.write_result(-ex.errno)
            return
        writer.write_result(0)
        writer.write_str(link)

    def symlink(self, reader, writer):
        source = reader.read_str()
        target = reader.read_path(self.root)
        result = 0
        try:
            os.symlink(source, target)
        except OSError as ex:
            result = -ex.errno
        writer.write_result(result)

    def link(self, reader, writer):
        source = reader.read_path(self.root)
        target = reader.read_path(self.root)
        result = 0
        try:
            os.link(source, target)
        except OSError as ex:
            result = -ex.errno
        writer.write_result(result)

    def rename(self, reader, writer):
        source = reader.read_path(self.root)
        target = reader.read_path(self.root)
        flags = reader.read_rename_flags()
        result = 0
        try:
            if RENAME_EXCHANGE == (flags & RENAME_EXCHANGE):
                # exchange is not supported
                result = -errno.EINVAL
            elif RENAME_NOREPLACE == (flags & RENAME_NOREPLACE):
                os.rename(source, target)
            else:
                os.replace(source, target)
        except OSError as ex:
            result = -ex.errno
        writer.write_result(result)

    def chmod(self, reader, writer):
        path = reader.read_path(self.root)
        mode = reader.read_mode()
        result = 0
        try:
            os.chmod(path, mode)
        except OSError as ex:
            result = -ex.errno
        writer.write_result(result)

    def chown(self, reader, writer):
        path = reader.read_path(self.root)
        uid = reader.read_u32()
        gid = reader.read_u32()
        result = 0
        try:
            os.chown(path, uid, gid)
        except OSError as ex:
            result = -ex.errno
        writer.write_result(result)

    def truncate(self, reader, writer):
        path = reader.read_path(self.root)
        size = reader.read_u64()
        fd = reader.read_u64()
        result = 0
        try:
            if fd != INVALID_FD:
                os.ftruncate(fd, size)
            else:
                os.truncate(path, size)
        except OSError as ex:
            result = -ex.errno
        writer.write_result(result)

    def fsync(self, reader, writer):
        _ = reader.read_path(self.root)
        _ = reader.read_bool()
        fd = reader.read_u64()
        result = 0
        try:
            os.fsync(fd)
        except OSError as ex:
            result = -ex.errno
        writer.write_result(result)

    def utimens(self, reader, writer):
        path = reader.read_path(self.root)
        atime = reader.read_u64()
        atime_ns = reader.read_u32()
        mtime = reader.read_u64()
        mtime_ns = reader.read_u32()
        result = 0
        try:
            os.utime(path, (atime, mtime), ns = (atime_ns, mtime_ns))
        except OSError as ex:
            result = -ex.errno
        writer.write_result(result)

    def open(self, reader, writer):
        path = reader.read_path(self.root)
        flags = reader.read_openflags()
        try:
            fd = os.open(path, flags)
        except OSError as ex:
            writer.write_result(-ex.errno)
            return
        writer.write_result(0)
        writer.write_u64(fd)

    def mknod(self, reader, writer):
        path = reader.read_path(self.root)
        mode = reader.read_mode()
        rdev = reader.read_u64()
        result = 0
        try:
            os.mknod(path, mode, rdev)
        except OSError as ex:
            result = -ex.errno
        writer.write_result(result)

    def create(self, reader, writer):
        path = reader.read_path(self.root)
        mode = reader.read_mode()
        try:
            flags = os.O_CREAT | os.O_WRONLY | os.O_TRUNC
            fd = os.open(path, flags, mode)
        except OSError as ex:
            writer.write_result(-ex.errno)
            return
        writer.write_result(0)
        writer.write_u64(fd)

    def release(self, reader, writer):
        _ = reader.read_path(self.root)
        fd = reader.read_u64()
        result = 0
        try:
            os.close(fd)
        except OSError as ex:
            writer.write_result(-ex.errno)
            return
        writer.write_result(result)

    def unlink(self, reader, writer):
        path = reader.read_path(self.root)
        result = 0
        try:
            os.unlink(path)
        except OSError as ex:
            result = -ex.errno
        writer.write_result(result)

    def read(self, reader, writer):
        path = reader.read_path(self.root)
        size = reader.read_u32()
        offset = reader.read_u64()
        fd = reader.read_u64()
        try:
            if fd != INVALID_FD:
                buffer = os.pread(fd, size, offset)
            else:
                with os.open(path, os.O_RDONLY) as f:
                    buffer = os.pread(f, size, offset)
            writer.write_result(len(buffer))
            writer.write_bytes(buffer)
        except OSError as ex:
            writer.write_result(-ex.errno)

    def write(self, reader, writer):
        path = reader.read_path(self.root)
        data = reader.read_bytes()
        offset = reader.read_u64()
        fd = reader.read_u64()
        result = 0
        try:
            if fd != INVALID_FD:
                result = os.pwrite(fd, data, offset)
            else:
                with os.open(path, os.O_WRONLY) as f:
                    result = os.pwrite(f, data, offset)
        except OSError as ex:
            result = -ex.errno
        writer.write_result(result)

    def mkdir(self, reader, writer):
        path = reader.read_path(self.root)
        mode = reader.read_u32()
        result = 0
        try:
            os.mkdir(path, mode)
        except OSError as ex:
            result = -ex.errno
        writer.write_result(result)

    def readdir(self, reader, writer):
        path = reader.read_path(self.root)
        names = []
        try:
            with os.scandir(path) as entries:
                for entry in entries:
                    names.append(entry.name)
        except OSError as ex:
            writer.write_result(-ex.errno)
            return
        writer.write_result(0)
        writer.write_strings(names)

    def rmdir(self, reader, writer):
        path = reader.read_path(self.root)
        result = 0
        try:
            os.rmdir(path)
        except OSError as ex:
            result = -ex.errno
        writer.write_result(result)

    def statfs(self, reader, writer):
        path = reader.read_path(self.root)
        try:
            buffer = os.statvfs(path)
        except OSError as ex:
            writer.write_result(-ex.errno)
            return
        writer.write_result(0)
        writer.write_u64(buffer.f_bsize)
        writer.write_u64(buffer.f_frsize)
        writer.write_u64(buffer.f_blocks)
        writer.write_u64(buffer.f_bfree)
        writer.write_u64(buffer.f_bavail)
        writer.write_u64(buffer.f_files)
        writer.write_u64(buffer.f_ffree)
        writer.write_u64(buffer.f_namemax)

    def getcreds(self, _, writer):
        credentials = self.token if self.token != None and self.token != "" else getpass.getpass(prompt="credentials: ")
        writer.write_str(credentials)

def main():
    parser = argparse.ArgumentParser(prog='webfuse_provider')
    parser.add_argument('-p', '--path', type=str, required=False, default='.',
        help='path to provide (default: \".\")')
    parser.add_argument('-u', '--url', type=str, required=True,
        help='URL of webfuse server, e.g. \"ws://localhost:8081\"')
    parser.add_argument('-t', '--token', type=str, required=False, default='',
        help='authentication token (default: contents of environment variable WEBFUSE_TOKEN)')
    args = parser.parse_args()
    token = args.token if args.token != "" else os.environ.get('WEBFUSE_TOKEN')
    provider = FilesystemProvider(args.path, args.url, token)
    try:
        asyncio.run(provider.run())
    except KeyboardInterrupt as _:
        pass

if __name__ == '__main__':
    main()
