#ifndef WF_CONTAINER_OF_H
#define WF_CONTAINER_OF_H

#ifndef __cplusplus
#include <stddef.h>
#else
#include <cstddef>
#endif

#define WF_CONTAINER_OF(pointer, type, member) \
    ({ \
        const typeof( ((type *)0)->member ) *__member = (pointer); \
        (type *)( (char *)__member - offsetof(type, member) ); \
    })

#endif
