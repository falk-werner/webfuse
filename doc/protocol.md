# Webufse 2 Protocol

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


## Message



## Methods

### access

| Field | Data Type        | Description |
| ----- | ---------------- | ----------- |
|       | uint32           | message id  |
| type  | uint8            | message type (0x00) |
| path  | string           | |
| mode  | access_mode (i8) |

#### Response

| Field |