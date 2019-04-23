#ifndef wf_container_of_H
#define wf_container_of_H

#ifndef __cplusplus
#include <stddef.h>
#else
#include <cstddef>
#endif

#ifdef __GNUC__
#define wf_container_of(pointer, type, member) \
    ({ \
        const typeof( ((type *)0)->member ) * __member = (pointer); \
        (type *)( (char *)__member - offsetof(type, member) ); \
    })
#else
#define wf_container_of(pointer, type, member) \
    (type *)  (((char *) pointer) - offsetof(type, member))
#endif

#endif
