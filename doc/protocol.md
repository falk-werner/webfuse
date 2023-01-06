# Webfuse 2 Protocol

## Scope

This document describes the webfuse 2 communication protocol. The protocol is used to transfer messages between a `webfuse2 service` and a `webfuse2 provider`. In contrast to `legacy webfuse`, which is based on `JSON RPC` the `webfuse2 protocol` is a binary protocol.

## Definitions

### Webfuse2 Service

A `webfuse2 service` is both,
- a [websocket](https://en.wikipedia.org/wiki/WebSocket) service providing the `webfuse2` protocol
- a [fuse](https://github.com/libfuse/libfuse) filesystem attached to a local mountpoint

The `webfuse2 service` awaits incoming connections from a `webfuse2 provider`. Once connected, it communicates all the filesystem requests originated by the `libfuse` to the connected `webfuse2 provider` using the `websocket`-based `webfuse2 protocol`.

By doing so, `webfuse2` allows to inject a filesystem to a remote device.

### Webfuse2 Provider

A `webfuse2 provider` provides a filesystem to a remote device using the `websocket`-based `webfuse2 protocol`. Therefore, a `webfuse2 provider` implements a `websocket` client.

## Message exchange

Once connected, the `webfuse2 protocol` implements a strict request-response scheme, where
- all requests are send by the `webfuse2 service`,
- all requests require a response and
- all responses are send by the `webfuse2 provider`

Note that this communication is reversed the typical client-server-communication scheme. In `webfuse2` the `webfuse2 serive` (server) sends all the requests and the `webfuse provider` (client) sends the responses.

For message transfer, the [websocket](https://en.wikipedia.org/wiki/WebSocket) protocol is used. All messages are in binary form, plain text messages are never used by the `webfuse2 protocol`.


## Endianness

All numeric data types are transferred in [Big Endian](https://en.wikipedia.org/wiki/Endianness).  
For instance, the uint32 value 1 will be transferred as

    00 00 00 01

## Data Types

### Basic data types

| Data Type | Width  | Description |
| --------- | ------ | ----------- |
| bool      |  8 bit | boolean value |
| u8        |  8 bit | 8 bit unsigned integer |
| u32       | 32 bit | 32 bit unsigned integer |
| u64       | 64 bit | 64 bit unsigned integer | 
| i32       | 32 bit | 32 bit signed integer |

### Derrived integral types

| Data Type | Base Type | Description |
| --------- | --------- | ----------- |
| uid       | u32       | user ID     |
| gid       | u32       | group ID    |
| dev       | u64       | device ID   |
| handle    | u64       | file handle |

### Flags and Enums

| Data Type    | Base Type | Description               |
| ------------ | --------- | ------------------------- |
| result       | i32       | result type of methods    |
| access_mode  | u32       | mode of `access` method   |
| mode         | u32       | file type and permissions |
| open_flags   | i32       | flags of `open` method    |
| rename_flags | u8        | flags of `rename` method  |

#### result

| Value Range    | Description            |
| -------------- | ---------------------- |
| 0              | method call succeed    |
| negative value | error code (see below) |
| positive value | amount of bytes read or written (`read` and `write` only) |

| Error Code   | Value | Description |
| ------------ | ----- | ----------- |
| E2BIG        |   -7  | argument list too long |
| EACCES       |  -13  | permission denied |
| EAGAIN       |  -11  | resource temporarily unavailable |
| EBADF        |   -9  | bad file descriptor |
| EBUSY        |  -16  | device or resource busy |
| EDESTADDRREQ |  -89  | destination address required |
| EDQUOT       | -122  | disk quota exceeded |
| EEXIST       |  -17  | file exists |
| EFAULT       |  -14  | bad address |
| EFBIG        |  -27  | file too large |
| EINTR        |   -4  | interrupt function call |
| EINVAL       |  -22  | invalid argument |
| EIO          |   -5  | input / output error |
| EISDIR       |  -21  | is a directory
| ELOOP        |  -40  | too many levels of symbolic links |
| EMFILE       |  -24  | too many open files |
| EMLINK       |  -31  | too many links |
| ENAMETOOLONG |  -36  | filename too long |
| ENFILE       |  -23  | too many open files in system |
| ENODATA      |  -61  | the named attribute does not exist, or the process has not access to this attribute |
| ENODEV       |  -19  | no such device |
| ENOENT       |   -2  | no such file or directory |
| ENOMEM       |  -12  | not enough space / cannot allocate memory |
| ENOSPC       |  -28  | no space left on device |
| ENOSYS       |  -38  | function not implemented |
| ENOTDIR      |  -20  | not a directory |
| ENOTEMPTY    |  -39  | directory not empty |
| ENOTSUP      |  -95  | operation not supported |
| ENXIO        |   -6  | no such device or address |
| EOVERFLOW    |  -75  | value too large to be stored in data type |
| EPERM        |   -1  | operation not permitted |
| EPIPE        |  -32  | broken pipe |
| ERANGE       |  -34  | result too large |
| EROFS        |  -30  | read-only filesystem |
| ETXTBSY      |  -26  | text file busy |
| EXDEV        |  -18  | improper link |
| EWOULDBLOCK  |  -11  | resource temporarily unavailable |

#### access mode

| Mode | Value | Description |
| ---- | ----- | ----------- |
| F_OK | 0     | Tests, whether the file exists |
| X_OK | 1     | Tests, whether the file is executable |
| W_OK | 2     | Tests, whether the file is writable |
| R_OK | 4     | Tests, whether the file is readable |

#### mode

_Note that the following numbers are in `octal` notation._

| Fields and Flags | Mask     | Description |
| ---------------- | -------- | ----------- |
| Protection mask  | 0o000777 | Cointains the file protection flags (rwx for owner, group and others) |
| Sticky mask      | 0o007000 | Sticky bits |
| S_ISVTX          | 0o001000 | Sticky bit |
| S_ISGID          | 0o002000 | Set-Group-ID bit |
| S_ISUID          | 0o004000 | Set-User-ID bit |
| Filetype mask    | 0o170000 | Filetype mask |
| S_IFREG          | 0o100000 | regular file |
| S_IFDIR          | 0o040000 | directory |
| S_IFCHR          | 0o020000 | character device |
| S_IFBLK          | 0o060000 | block device |
| S_IFIFO          | 0o010000 | named pipe |
| S_IFLNK          | 0o120000 | link |
| S_IFSOCK         | 0o140000 | socket |

#### open_flags

_Note that the following numbers are in `octal` notation._

| Flags            | Value    | Description |
| ---------------- | -------- | ----------- |
| O_ACCMODE        | 0o03     | Access mode mask |
| O_RDONLY         | 0o00     | open file read-only |
| O_WRONLY         | 0o01     | open file write-only |
| O_RDWR           | 0o02     | open file for reading and writing |
| O_APPEND         | 0o000002000 | open file in append mode |
| O_ASYNC          | 0o000020000 | enable signal-driven I/O |
| O_CLOEXEC        | 0o002000000 | enable close-on-exec |
| O_CREAT          | 0o000000100 | create file if path does not exists |
| O_DIRECT         | 0o000040000 | try to minimize cache effects on I/O |
| O_DIRECTORY      | 0o000200000 | open a directory |
| O_DSYNC          | 0o000010000 | write with synchronized I/O data integrity |
| O_EXCL           | 0o000000200 | ensure that file exists when specified in conjunction with O_CREATE |
| O_LARGEFILE      | 0o000100000 | allow large files to be opened |
| O_NOATIME        | 0o001000000 | do not update file last access time |
| O_NOCTTY         | 0o000000400 | make device the process's controlling terminal |
| O_NOFOLLOW       | 0o000400000 | fail to open, if basename is a symbolic link |
| O_NONBLOCK       | 0o000004000 | open in nonblocking mode |
| O_NDELAY         | 0o000004000 | open in nonblocking mode |
| O_PATH           | 0o010000000 | see manual entry of `open(2)` for details |
| O_SYNC           | 0o004010000 | write using synchronized I/O |
| O_TMPFILE        | 0o020200000 | create an unnamed temporary file |
| O_TRUNC          | 0o000001000 | truncate fole to length 0 |

#### rename_flags

| Flag             | Value | Description |
| ---------------- | ----- | ----------- |
| RENAME_NOREPLACE | 1     | do not overwrite the new file |
| RENAME_EXCHANGE  | 2     | atomically exchange the files |  

### Complex Types

| Data Type  | Description           |
| ---------- | --------------------- |
| string     | UTF-8 string          |
| bytes      | array of bytes        |
| timestamp  | date and time         |
| attributes | file attributes       |
| statistics | filesystem statistics |

#### string

| Field | Data Type | Description                   |
| ----- | --------- | ----------------------------- |
| size  | u32       | length of the string in bytes |
| data  | u8[]      | string data (UTF-8)           |

#### bytes

| Field | Data Type | Description              |
| ----- | --------- | ------------------------ |
| size  | u32       | length of the byte array |
| data  | u8[]      | array data               |

#### timestamp

| Field   | Data Type | Description                   |
| ------- | --------- | ----------------------------- |
| seconds | u64       | seconds sind epoch (1.1.1970) |
| nsec    | u32       | nano seconds                  |

#### attributes

| Field  | Data Type  | Description                |
| ------ | ---------- | -------------------------- |
| inode  | u64        | Inode value                |
| nlink  | u64        | Number of hard links       |
| mode   | mode (u32) | file mode flags            |
| uid    | uid (u32)  | user ID                    |
| gid    | gid (u32)  | group ID                   |
| rdev   | dev (u64)  | device ID                  |
| size   | u64        | file size                  |
| blocks | u64        | number 512-byte blocks     |
| atime  | timestamp  | time of last access        |
| mtime  | timestamp  | time of last modification  |
| ctime  | timestamp  | time of last status change |

#### statistics

## Messages

| Field   | Type | Descripton |
| ------- | ---- | ---------- |
| id      | u32  | Unique ID of the message |
| type    | u8   | Type of the message |
| payload | u8[] | Payload according to the message type |

The `id` is just a number without any meaning for the `webfuse2 provider`. It is set by the `webfuse2 service` of a request and is copied by the `webfuse2 provider` to the response. A `webfuse2 service` implementation might choose to keep track on pending requests using the `id`.

### Erroneous Responses

Most responses contain a `result` encoding the status of the operation. While successful responses may contain additional data, erroneous responses must not be decoded by a `webfuse2 service` implementation beyond the `result` value.

### Message Types

_Note that the following numbers are in `hexadecimal` notation._

| Method   | Request | Response |
| -------- | ------- | -------- |
| access   | 0x01    | 0x81     |
| getattr  | 0x02    | 0x82     |
| readlink | 0x03    | 0x83     |
| symlink  | 0x04    | 0x84     |
| link     | 0x05    | 0x85     |
| rename   | 0x06    | 0x86     |
| chmod    | 0x07    | 0x87     |
| chown    | 0x08    | 0x88     |
| truncate | 0x09    | 0x89     |
| fsync    | 0x0a    | 0x8a     |
| open     | 0x0b    | 0x8b     |
| mknod    | 0x0c    | 0x8c     |
| create   | 0x0d    | 0x8d     |
| release  | 0x0e    | 0x8e     |
| unlink   | 0x0f    | 0x8f     |
| read     | 0x10    | 0x90     |
| write    | 0x11    | 0x91     |
| mkdir    | 0x12    | 0x92     |
| readdir  | 0x13    | 0x93     |
| rmdir    | 0x14    | 0x94     |
| statfs   | 0x15    | 0x95     |
| utimens  | 0x16    | 0x96     |

## Methods

### access

#### Request

| Field | Data Type        | Description |
| ----- | ---------------- | ----------- |
| id    | u32              | message id  |
| type  | u8               | message type (0x01) |
| path  | string           | |
| mode  | access_mode (i8) |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |
| result | result    | operation status |

### getattr

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |
| result | result    | operation status |

### readlink

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |
| result | result    | operation status |

### symlink

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |
| result | result    | operation status |

### link

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |
| result | result    | operation status |

### rename

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |
| result | result    | operation status |

### chmod

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |
| result | result    | operation status |

### chown

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |
| result | result    | operation status |

### truncate

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |
| result | result    | operation status |

### fsync

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |
| result | result    | operation status |

### open

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |
| result | result    | operation status |

### mknod

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |
| result | result    | operation status |

### create

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |
| result | result    | operation status |

### release

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |
| result | result    | operation status |

### unlink

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |
| result | result    | operation status |

### read

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |
| result | result    | operation status |

### write

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |
| result | result    | operation status |

### mkdir

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |
| result | result    | operation status |

### readdir

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |
| result | result    | operation status |

### rmdir

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |
| result | result    | operation status |

### statfs

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |
| result | result    | operation status |

### utimens

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |
| result | result    | operation status |
