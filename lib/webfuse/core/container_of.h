#ifndef WF_CONTAINER_OF_H
#define WF_CONTAINER_OF_H

#ifndef __cplusplus
#include <stddef.h>
#else
#include <cstddef>
#endif

#ifdef __GNUC__
#define WF_CONTAINER_OF(pointer, type, member) \
    ({ \
        const typeof( ((type *)0)->member ) * __member = (pointer); \
        (type *)( (char *)__member - offsetof(type, member) ); \
    })
#else
#define WF_CONTAINER_OF(pointer, type, member) \
    (type *)  (((char *) pointer) - offsetof(type, member))
#endif

#endif
