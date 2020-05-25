#include "webfuse/utils/path.h"
#include <stdlib.h>
#include <string.h>

#define WF_PATH_DEFAULT_CAPACITY (8)

struct wf_path
{
    char * * elements;
    size_t count;
    size_t capacity;
};

static void 
wf_path_add(
    struct wf_path * path,
    char const * element,
    size_t element_size)
{
    if (0 < element_size)
    {
        if (path->count >= path->capacity)
        {
            size_t new_capacity = 2 * path->capacity;
            size_t new_size = sizeof(char*) * new_capacity;

            char * * elements = realloc(path->elements, new_size);
            if (NULL != elements)
            {
                path->elements = elements;
                path->capacity = new_capacity;
            }
        }

        if (path->count < path->capacity)
        {
            path->elements[path->count] = strndup(element, element_size);
            path->count++;
        }
    }
}

struct wf_path *
wf_path_create(
    char const * value)
{
    struct wf_path * path = malloc(sizeof(struct wf_path));
    path->elements = malloc(sizeof(char*) * WF_PATH_DEFAULT_CAPACITY);
    path->capacity = WF_PATH_DEFAULT_CAPACITY;
    path->count = 0;

    char const * remainder = value;
    char const * pos = strchr(remainder, '/');
    while (NULL != pos)
    {
        wf_path_add(path, remainder, (pos - remainder));
        remainder = pos + 1;
        pos = strchr(remainder, '/');
    }

    wf_path_add(path, remainder, strlen(remainder));

    return path;
}

void
wf_path_dispose(
    struct wf_path * path)
{
    for(size_t i = 0; i < path->count; i++)
    {
        free(path->elements[i]);
    }

    free(path->elements);
    free(path);
    (void) path;
}

size_t
wf_path_element_count(
    struct wf_path * path)
{
    return path->count;
}

char const *
wf_path_get_element(
    struct wf_path * path,
    size_t i)
{  
    char const * result = NULL;
    if (i < path->count)
    {
        result = path->elements[i];
    }

    return result;
}

char const *
wf_path_get_filename(
    struct wf_path * path)
{
    char const * result = NULL;

    if (0 < path->count)
    {
        result = path->elements[path->count - 1];
    }

    return result;
}
