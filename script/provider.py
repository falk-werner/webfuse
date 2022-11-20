#!/usr/bin/env python3

import asyncio
import os
import stat
import websockets
import errno

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


class MessageReader:
    def __init__(self, buffer):
        self.buffer = buffer
        self.offset = 0

    def read_u8(self):
        value = self.buffer[self.offset]
        self.offset += 1
        return value
    
    def read_u32(self):
        value = (self.buffer[self.offset] << 24) + (self.buffer[self.offset + 1] << 16) + (self.buffer[self.offset + 2] << 8) + self.buffer[self.offset + 3]
        self.offset += 4
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
        mode += os.R_OK if R_OK == (value & R_OK) else 0
        mode += os.W_OK if W_OK == (value & W_OK) else 0
        mode += os.X_OK if X_OK == (value & X_OK) else 0
        return mode

    def read_rename_flags(self):
        return self.read_u8()
    
    def read_mode(self):
        value = self.read_u32()
        mode = value & 0o7777
        mode += stat.S_IFREG  if MODE_REG  == (value & MODE_REG ) else 0
        mode += stat.S_IFDIR  if MODE_DIR  == (value & MODE_DIR ) else 0
        mode += stat.S_IFCHR  if MODE_CHR  == (value & MODE_CHR ) else 0
        mode += stat.S_IFBLK  if MODE_BLK  == (value & MODE_BLK ) else 0
        mode += stat.S_IFFIFO if MODE_FIFO == (value & MODE_FIFO) else 0
        mode += stat.S_IFLNK  if MODE_LNK  == (value & MODE_LNK ) else 0
        mode += stat.S_IFSOCK if MODE_SOCK == (value & MODE_SOCK) else 0
        return mode



class MessageWriter:
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
            if value in ERRNO:
                value = ERRNO[value]
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
        

class FilesystemProvider:
    def __init__(self, path, url):
        self.root = os.path.abspath(path)
        self.url = url
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
            0x15: FilesystemProvider.statfs             
        }
    
    async def run(self):
        async with websockets.connect(self.url) as connection:
            while True:
                request = await connection.recv()
                reader = MessageReader(request)
                message_id = reader.read_u32()
                message_type = reader.read_u8()
                print("received message: id=%d, type=%d" % (message_id, message_type))
                writer = MessageWriter(message_id, RESPONSE + message_type)
                if message_type in self.commands:
                    method = self.commands[message_type]
                    method(self, reader, writer)
                else:
                    print("unknown message type: %d" % message_type)
                response = writer.get_bytes()
                await connection.send(response)

    def access(self, reader, writer):
        path = reader.read_path(self.root)
        mode = reader.read_access_mode()
        result = -errno.EACCES
        try:
            if os.access(path, mode) == True:
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
            if fd != 0xffffffffffffffff:
                os.ftruncate(fd, size)
            else:
                os.truncate(path, size)
        except OSError as ex:
            result = -ex.errno
        writer.write_result(result)

    def fsync(self, reader, writer):
        path = reader.read_path(self.root)
        _ = reader.read_i32()
        fd = reader.read_u64()
        result = 0
        try:
            os.fsync(fd)
        except OSError as ex:
            result = -ex.errno
        writer.write_result(result)

    def open(self, reader, writer):
        pass

    def mknod(self, reader, writer):
        pass

    def create(self, reader, writer):
        pass

    def release(self, reader, writer):
        pass

    def unlink(self, reader, writer):
        pass

    def read(self, reader, writer):
        pass

    def write(self, reader, writer):
        pass

    def mkdir(self, reader, writer):
        pass

    def readdir(self, reader, writer):
        path = reader.read_path(self.root)
        names = []
        try:
            with os.scandir(path) as it:
                for entry in it:
                    names.append(entry.name)
        except OSError as ex:
            writer.write_result(-ex.errno)
            return
        writer.write_result(0)
        writer.write_strings(names)

    def rmdir(self, reader, writer):
        pass

    def statfs(self, reader, writer):
        pass      
        

if __name__ == '__main__':
    provider = FilesystemProvider('.', 'ws://localhost:8081')
    asyncio.run(provider.run())
