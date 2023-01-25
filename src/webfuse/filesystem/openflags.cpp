#include "webfuse/filesystem/openflags.hpp"
#include <fcntl.h>

namespace webfuse
{

openflags::openflags(int32_t value)
: value_(value)
{

}

openflags::operator int32_t() const
{
    return value_;
}

openflags openflags::from_int(int value)
{
    int32_t result = 0;

    if (O_RDONLY    == (value & O_RDONLY   )) { result |= openflags::rdonly;    }
    if (O_WRONLY    == (value & O_WRONLY   )) { result |= openflags::wronly;    }
    if (O_RDWR      == (value & O_RDWR     )) { result |= openflags::rdwr;      }

    if (O_APPEND    == (value & O_APPEND   )) { result |= openflags::append;    }
    if (O_ASYNC     == (value & O_ASYNC    )) { result |= openflags::async;     }
    if (O_CLOEXEC   == (value & O_CLOEXEC  )) { result |= openflags::cloexec;   }
    if (O_CREAT     == (value & O_CREAT    )) { result |= openflags::creat;     }
    if (O_DIRECT    == (value & O_DIRECT   )) { result |= openflags::direct;    }
    if (O_DIRECTORY == (value & O_DIRECTORY)) { result |= openflags::directory; }
    if (O_DSYNC     == (value & O_DSYNC    )) { result |= openflags::dsync;     }
    if (O_EXCL      == (value & O_EXCL     )) { result |= openflags::excl;      }
    if (O_LARGEFILE == (value & O_LARGEFILE)) { result |= openflags::largefile; }
    if (O_NOATIME   == (value & O_NOATIME  )) { result |= openflags::noatime;   }
    if (O_NOCTTY    == (value & O_NOCTTY   )) { result |= openflags::noctty;    }
    if (O_NOFOLLOW  == (value & O_NOFOLLOW )) { result |= openflags::nofollow;  }
    if (O_NONBLOCK  == (value & O_NONBLOCK )) { result |= openflags::nonblock;  }
    if (O_NDELAY    == (value & O_NDELAY   )) { result |= openflags::ndelay;    }
    if (O_PATH      == (value & O_PATH     )) { result |= openflags::path;      }
    if (O_SYNC      == (value & O_SYNC     )) { result |= openflags::sync;      }
    if (O_TMPFILE   == (value & O_TMPFILE  )) { result |= openflags::tmpfile;     }
    if (O_TRUNC     == (value & O_TRUNC    )) { result |= openflags::trunc;     }

    return openflags(result);
}

int openflags::to_int() const
{
    int result = 0;

    if (openflags::rdonly    == (value_ & openflags::rdonly   )) { result |= O_RDONLY;    }
    if (openflags::wronly    == (value_ & openflags::wronly   )) { result |= O_WRONLY;    }
    if (openflags::rdwr      == (value_ & openflags::rdwr     )) { result |= O_RDWR;      }

    if (openflags::append    == (value_ & openflags::append   )) { result |= O_APPEND;    }
    if (openflags::async     == (value_ & openflags::async    )) { result |= O_ASYNC;     }
    if (openflags::cloexec   == (value_ & openflags::cloexec  )) { result |= O_CLOEXEC;   }
    if (openflags::creat     == (value_ & openflags::creat    )) { result |= O_CREAT;     }
    if (openflags::direct    == (value_ & openflags::direct   )) { result |= O_DIRECT;    }
    if (openflags::directory == (value_ & openflags::directory)) { result |= O_DIRECTORY; }
    if (openflags::dsync     == (value_ & openflags::dsync    )) { result |= O_DSYNC;     }
    if (openflags::excl      == (value_ & openflags::excl     )) { result |= O_EXCL;      }
    if (openflags::largefile == (value_ & openflags::largefile)) { result |= O_LARGEFILE; }
    if (openflags::noatime   == (value_ & openflags::noatime  )) { result |= O_NOATIME;   }
    if (openflags::noctty    == (value_ & openflags::noctty   )) { result |= O_NOCTTY;    }
    if (openflags::nofollow  == (value_ & openflags::nofollow )) { result |= O_NOFOLLOW;  }
    if (openflags::nonblock  == (value_ & openflags::nonblock )) { result |= O_NONBLOCK;  }
    if (openflags::ndelay    == (value_ & openflags::ndelay   )) { result |= O_NDELAY;    }
    if (openflags::path      == (value_ & openflags::path     )) { result |= O_PATH;      }
    if (openflags::sync      == (value_ & openflags::sync     )) { result |= O_SYNC;      }
    if (openflags::tmpfile   == (value_ & openflags::tmpfile  )) { result |= O_TMPFILE;   }
    if (openflags::trunc     == (value_ & openflags::trunc    )) { result |= O_TRUNC;     }

    return result;
}


}
