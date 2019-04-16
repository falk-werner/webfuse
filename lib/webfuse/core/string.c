#include "webfuse/core/string.h"

#include <stdio.h>
#include <stdlib.h>

char * wf_create_string(char const * format, ...)
{
    char * result = NULL;

    va_list measure_args;
    va_start(measure_args, format);
    char buffer;
    int needed = vsnprintf(&buffer, 1, format, measure_args);
    va_end(measure_args);

    if (0 <= needed)
    {
        result = malloc(needed + 1);
        if (NULL != result)
        {
            va_list args;
            va_start(args, format);
            int count = vsnprintf(result, needed + 1, format, args);
            va_end(args);

            if ((count < 0) || (needed < count))
            {
                free(result);
                result = NULL;
            }
        }
    }

    return result;
}
