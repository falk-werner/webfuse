# Webfuse2 Protocol

## Scope

This document describes the webfuse 2 communication protocol. The protocol is used to transfer messages between a `webfuse service` and a `webfuse provider`. In contrast to `legacy webfuse`, which is based on `JSON RPC` the `webfuse2 protocol` is a binary protocol.

## Definitions

### Webfuse Service

A `webfuse service` is both,
- a [websocket](https://en.wikipedia.org/wiki/WebSocket) service providing the `webfuse` protocol
- a [fuse](https://github.com/libfuse/libfuse) filesystem attached to a local mountpoint

The `webfuse service` awaits incoming connections from a `webfuse provider`. Once connected, it communicates all the filesystem requests originated by the `libfuse` to the connected `webfuse provider` using the `websocket`-based `webfuse protocol`.

By doing so, `webfuse` allows to inject a filesystem to a remote device.

### Webfuse Provider

A `webfuse provider` provides a filesystem to a remote device using the `websocket`-based `webfuse protocol`. Therefore, a `webfuse provider` implements a `websocket` client.

## Websocket protocol name

The webfuse2 protocol uses the following websocket protocol name: `webfuse2`.

## Message exchange

Once connected, the `webfuse2 protocol` implements a strict request-response scheme, where
- all requests are send by the `webfuse service`,
- all requests require a response and
- all responses are send by the `webfuse provider`

Note that this communication is reversed the typical client-server-communication scheme. In `webfuse` the `webfuse service` (server) sends all the requests and the `webfuse provider` (client) sends the responses.

For message transfer, the [websocket](https://en.wikipedia.org/wiki/WebSocket) protocol is used. All messages are in binary form, plain text messages are never used by the `webfuse protocol`.


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
| strings    | list of strings       |
| bytes      | array of bytes        |
| timestamp  | date and time         |
| attributes | file attributes       |
| statistics | filesystem statistics |

#### string

| Field | Data Type | Description                   |
| ----- | --------- | ----------------------------- |
| size  | u32       | length of the string in bytes |
| data  | u8[]      | string data (UTF-8)           |

#### strings

| Field | Data Type | Description                       |
| ----- | --------- | --------------------------------- |
| size  | u32       | count of the elements in the list |
| data  | string[]  | strings                           |

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

| Field  | Data Type  | Description                |
| ------ | ---------- | -------------------------- |
| bsize  | u64        | Filesystem block size      |
| frsize | u64        | Fragment size              |
| blocks | u64        | Size of the filesystem if `frsize` units |
| bfree  | u64        | Number of free blocks      |
| bavail | u64        | Number of free blocks for unprivileged users |
| files  | u64        | Number of inodes           |
| ffree  | u64        | Number of free inodes      |
| namemax | u64       | Maximum filename length    |

## Messages

| Field   | Type | Descripton |
| ------- | ---- | ---------- |
| id      | u32  | Unique ID of the message |
| type    | u8   | Type of the message |
| payload | u8[] | Payload according to the message type |

The `id` is just a number without any meaning for the `webfuse provider`. It is set by the `webfuse service` of a request and is copied by the `webfuse provider` to the response. A `webfuse service` implementation might choose to keep track on pending requests using the `id`.

### Erroneous Responses

Most responses contain a `result` encoding the status of the operation. While successful responses may contain additional data, erroneous responses must not be decoded by a `webfuse service` implementation beyond the `result` value.

### Unknown requests

There are two reserved message types:
- **0x00:** Unknown request
- **0x80:** Unknown response

A `webfuse service` may send a request of type `unknown request` for conformance testing reasons.

Since each request requires a response, a `webfuse provider` must respond to any unknown requests with a message of `unknown response` type. This allows to add new request types in future.

### Accept additional data in requests

Both, a `webfuse provider` and a `webfuse service` must accept messages that contain more data than specified. This allows to add optional fields to existing requests and / or responses in future.

_Note there are no optional fields in the current revision of the `webfuse2 protocol` yet._

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

Since `webfuse` aims to communicate the `libfuse API` over a `websocket` connection, `webfuse` methods are tightly connected to [fuse operations](https://libfuse.github.io/doxygen/structfuse__operations.html) which itself have a tight connection to `posix filesystem operations`. Therefore, additional information about most `webfuse` operations can be found in the [fuse operations documentation](https://libfuse.github.io/doxygen/structfuse__operations.html) and / or the [man pages](https://man7.org/index.html).

### access

Checks the user's permissions for a file (see [man access(2)](https://man7.org/linux/man-pages/man2/access.2.html)).

#### Request

| Field | Data Type        | Description |
| ----- | ---------------- | ----------- |
| id    | u32              | message id  |
| type  | u8               | message type (0x01) |
| path  | string           | path of file to check |
| mode  | access_mode (i8) | access mode to check |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x81) |
| result | result    | operation status |

### getattr

Retrieve file attributes (see [man getattr(2)](https://man7.org/linux/man-pages/man2/getxattr.2.html)).

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x02) |
| path   | string    | path |


#### Response

| Field     | Data Type  | Description |
| --------- | ---------- | ----------- |
| id        | u32        | message id |
| type      | u8         | message type (0x82) |
| result    | result     | operation status |
| attibutes | attributes | attributes of file |

### readlink

Read value of a symbolik link (see [man readlink(2)](https://man7.org/linux/man-pages/man2/readlink.2.html)).

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id  |
| type   | u8        | message type (0x03) |
| path   | string    | path of link |


#### Response

| Field    | Data Type | Description |
| -------- | --------- | ----------- |
| id       | u32       | message id |
| type     | u8        | message type (0x83) |
| result   | result    | operation status |
| resolved | string    | resolved path |

### symlink

Make a new name of a file (see [man symlink(2)](https://man7.org/linux/man-pages/man2/symlink.2.html)).

#### Request

| Field    | Data Type | Description |
| -------- | --------- | ----------- |
| id       | u32       | message id |
| type     | u8        | message type (0x04) |
| target   | string    | target of link |
| linkpath | string    | name of the link |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x84) |
| result | result    | operation status |

### link

Make a new name for a file (see [man link(2)](https://man7.org/linux/man-pages/man2/link.2.html)).

#### Request

| Field    | Data Type | Description |
| -------- | --------- | ----------- |
| id       | u32       | message id |
| type     | u8        | message type (0x05) |
| old_path | string    | path of the existing file |
| new_path | string    | new name of the file |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x85) |
| result | result    | operation status |

### rename

Change the name of a file (see [man rename(2)](https://man7.org/linux/man-pages/man2/rename.2.html)).

#### Request

| Field    | Data Type | Description |
| -------- | --------- | ----------- |
| id       | u32       | message id |
| type     | u8        | message type (0x06) |
| old_path | string    | old name of the file |
| new_path | string    | new name of the file |
| flags    | rename_flags (u8) | flags to control the rename operation |

The following `flags` are defined:
- **0x00:** move the file from `old_path` to `new_path`
- **0x01 (RENAME_NOREPLACE):** do not override `new_path`  
  This results in an error, when `new_path` already exists.
- **0x02 (RENAME_EXCHANGE):** atomically exchange the files

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x86) |
| result | result    | operation status |

### chmod

Change permissions of a file (see [man chmod(2)](https://man7.org/linux/man-pages/man2/chmod.2.html)).

#### Request

| Field  | Data Type  | Description |
| ------ | ---------- | ----------- |
| id     | u32        | message id |
| type   | u8         | message type (0x07) |
| path   | string     | path of the file |
| mode   | mode (u32) | new file permissions |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x87) |
| result | result    | operation status |

### chown

Change ownership of a file (see [man chown(2)](https://man7.org/linux/man-pages/man2/chown.2.html)).

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x08) |
| path   | string    | path of the file |
| uid    | uid (u32) | user id of the new owner |
| gid    | gid (u32) | group id of the new owning group |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x88) |
| result | result    | operation status |

### truncate

Truncate a file to a specified length (see [man truncate(2)](https://man7.org/linux/man-pages/man2/truncate64.2.html)).

#### Request

| Field  | Data Type    | Description |
| ------ | ------------ | ----------- |
| id     | u32          | message id |
| type   | u8           | message type (0x09) |
| path   | string       | path of the file |
| size   | u64          | new file size |
| handle | handle (u64) | handle of the file |

_Note that handle might be invalid (-1), even if the file is open._

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x89) |
| result | result    | operation status |

### fsync

Sychronize a file's in-core state with storage device (see [man fsync(2)](https://man7.org/linux/man-pages/man2/fsync.2.html)).

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x0a) |
| path   | string    | path of the file |
| is_datasync | bool | if true, sync only user data |
| handle | handle (u64) | handle of the file |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x8a) |
| result | result    | operation status |

### open

Open and possibly create a file ([man open(2)](https://man7.org/linux/man-pages/man2/open.2.html)).

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x0b) |
| path   | string    | path of the file |
| flags  | open_flags (i32) | flags |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x8b) |
| result | result    | operation status |
| handle | handle (u64) | handle of the file |

### mknod

Create a special or ordinary file (see [man mknod(2)](https://man7.org/linux/man-pages/man2/mknod.2.html)).

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x0c) |
| path   | string    | path of the file |
| mode   | mode (u32) | mode of the file |
| dev    | dev (64)  | device type |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x8c) |
| result | result    | operation status |

### create

Create a new file or rewrite an existing one (see [man creat(3p)](https://man7.org/linux/man-pages/man3/creat.3p.html)).

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x0d) |
| path   | string    | path of the file |
| mode   | mode (u32) | mode of the file |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x8d) |
| result | result    | operation status |
| handle | handle (u64) | handle of the file |

### release

Releases a file handle (see [man close(2)](https://man7.org/linux/man-pages/man2/close.2.html)).

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x0e) |
| path   | string    | path of the file |
| handle | handle (u64) | handle of the file |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x8e) |
| result | result    | operation status |

### unlink

Delete a name and possibly the file it refers to ([man unlink(2)](https://man7.org/linux/man-pages/man2/unlink.2.html)).

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x0f) |
| path   | string    | path of the file |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x8f) |
| result | result    | operation status |

### read

Read from a file description (see [man read(2)](https://man7.org/linux/man-pages/man2/read.2.html), [man pread(2)](https://man7.org/linux/man-pages/man2/pread.2.html)).

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x10) |
| path   | string    | path of the file |
| buffer_size | u32  | max. amount of bytes requested |
| offset | u64       | offset of the file |
| handle | handle (u64) | handle of the file |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x90) |
| result | result    | amount of byte read or error code |
| data   | bytes     | requested data |

_Note that results returns the amount of bytes read on success._

### write

Write to a file (see [man write(2)](https://man7.org/linux/man-pages/man2/write.2.html), [man pread(2)](https://man7.org/linux/man-pages/man2/pread.2.html)).

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x17) |
| data   | bytes     | data to write |
| offset | u64       | offset to write to |
| handle | handle (u64) | handle of the file |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x91) |
| result | result    | amount of bytes written or error code |

_Note that results returns the amount of bytes written on success._

### mkdir

Create a directory (see [man mkdir(2)](https://man7.org/linux/man-pages/man2/mkdir.2.html)).

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x12) |
| path   | string    | path of the directory |
| mode   | mode (u32) | directory permissions |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x92) |
| result | result    | operation status |

### readdir

Reads the contents of a directory.

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x13) |
| path   | string    | path of the directory |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x93) |
| result | result    | operation status |
| items  | strings   | names of the directory entries |

### rmdir

Delete a directory (see [man rmdir(2)](https://man7.org/linux/man-pages/man2/rmdir.2.html)).

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x14) |
| path   | string    | path of the directory |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x94) |
| result | result    | operation status |

### statfs

Get filesystem statistics (see [man statvfs(3)](https://man7.org/linux/man-pages/man3/statvfs.3.html)).

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x15) |
| path   | string    | path of the file |

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x95) |
| result | result    | operation status |
| statistics | statistics | filesystem statistics |

### utimens

Change the file timestamps with nanosecond precision ([man utimesat(2)](https://man7.org/linux/man-pages/man2/utimensat.2.html)).

#### Request

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x16) |
| path   | string    | path of the file |
| atime  | timestamp | new last access time |
| mtime  | timestamp | new last modified time |
| handle | handle (u64) | handle of the file |

_Note that handle might be invalid (-1), even if the file is open._

#### Response

| Field  | Data Type | Description |
| ------ | --------- | ----------- |
| id     | u32       | message id |
| type   | u8        | message type (0x96) |
| result | result    | operation status |

## Examples

### Get file attributes

````
service  -> provider:
    00 00 00 01 # message id   = 1
    02          # message type = getattr request
    00 00 00 01 # path.size    = 1
    '/'         # path         = "/"

provider -> service:
    00 00 00 01              # message id        = 1
    82                       # message type      = getattr response
    00 00 00 00              # result            = 0 (OK)
    00 00 00 00 00 00 00 01  # attributes.inode  = 1
    00 00 00 00 00 00 00 02  # attributes.nlink  = 2
    00 00 41 a4              # attributes.mode   = 0o40644 (S_IDDIR | 0o0644)
    00 00 03 e8              # attributes.uid    = 1000
    00 00 03 e8              # attributes.gid    = 1000
    00 00 00 00 00 00 00 00  # attributes.size   = 0
    00 00 00 00 00 00 00 00  # attributes.blocks = 0
    00 00 00 00 00 00 00 00  # attrbites.atime.sec  = 0
    00 00 00 00 00           # attributs.atime.nsec = 0
    00 00 00 00 00 00 00 00  # attrbites.mtime.sec  = 0
    00 00 00 00 00           # attributs.mtime.nsec = 0
    00 00 00 00 00 00 00 00  # attrbites.ctime.sec  = 0
    00 00 00 00 00           # attributs.ctime.nsec = 0
````

### Get file attributes (Failure)

_Note that attributs are skipped in case of an error._

````
service  -> provider:
    00 00 00 01 # message id   = 1
    02          # message type = getattr request
    00 00 00 04 # path.size    = 4
    "/foo"      # path         = "/foo"

provider -> service:
    00 00 00 01              # message id        = 1
    82                       # message type      = getattr response
    ff ff ff fe              # result            = -2 (ENOENT)
````

### List directory contents

_Note that '.' and '..' should not be included in the response._

````
service -> provider:
    00 00 00 02 # message id   = 2
    13          # message type = readdir request
    00 00 00 04 # path.size    = 4
    '/dir'      # path         = "/dir"

provider -> service:
    00 00 00 02 # message id    = 2
    93          # message type  = readdir response
    00 00 00 00 # result        = 0 (OK)
    00 00 00 03 # items.size    = 3
    00 00 00 03 # items[0].size = 3
    "foo"       # items[0]      = "foo"
    00 00 00 03 # items[0].size = 3
    "bar"       # items[0]      = "bar"
    00 00 00 03 # items[0].size = 3
    "baz"       # items[0]      = "baz"
````

### Unknown request

````
service -> provider:
    00 00 00 23 # message id   = 0x23
    42          # message type = ??? (not specified yet)
    ...         # some more data

provider -> service:
    00 00 00 23 # message id   = 0x23
    80          # message type = unknown response
````
