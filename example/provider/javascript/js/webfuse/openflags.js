
const OpenFlags = {
    ACCESS_MODE: 0x03,
    RDONLY : 0o00,
    WRONLY : 0o01,
    RDWR   : 0o02,

    APPEND    : 0o00002000,
    ASYNC     : 0o00020000,
    CLOEXEC   : 0o02000000,
    CREAT     : 0o00000100,
    DIRECT    : 0o00040000,
    DIRECTORY : 0o00200000,
    DSYNC     : 0o00010000,
    EXCL      : 0o00000200,
    LARGEFILE : 0o00100000,
    NOATIME   : 0o01000000,
    NOCTTY    : 0o00000400,
    NOFOLLOW  : 0o00400000,
    NONBLOCK  : 0o00004000,
    NDELAY    : 0o00004000,
    PATH      : 0o10000000,
    SYNC      : 0o04010000,
    TMPFILE   : 0o20200000,
    TRUNC     : 0o00001000
};

export { OpenFlags }