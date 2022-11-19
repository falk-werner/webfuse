#!/usr/bin/env python3

import asyncio
import os
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


class MessageWriter:
    def __init__(self, message_id, message_type):
        self.buffer = []
        self.write_u32(message_id)
        self.write_u8(message_type)
    
    def write_u8(self, value):
        self.buffer.append(value)
    
    def write_u32(self, value):
        a = (value >> 24) & 0xff
        b = (value >> 16) & 0xff
        c = (value >>  8) & 0xff
        d = value         & 0xff
        self.buffer.extend([a, b, c, d])

    def write_i32(self, value):
        self.write_u32(value & 0xffffffff)
    
    def write_result(self, value):
        if 0 > value:
            if value in ERRNO:
                value = ERRNO[value]
        self.write_i32(value)

    def get_bytes(self):
        return bytearray(self.buffer)
        

class FilesystemProvider:
    def __init__(self, path, url):
        self.root = os.path.abspath(path)
        self.url = url
        self.commands = {
            0x01: FilesystemProvider.access
        }
    
    async def run(self):
        async with websockets.connect(self.url) as connection:
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

if __name__ == '__main__':
    provider = FilesystemProvider('.', 'ws://localhost:8081')
    asyncio.run(provider.run())
