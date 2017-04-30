#include <stdio.h>
#include "generic.h"
#include "mem.h"

static bool _default_oom_handler(void *data)
{
    (void)data;
    fprintf(stderr, "seems we're gonna to be screwed ...\n");
    return false;
}

static oom_handler_t _oom_handler = _default_oom_handler;
static void *_oom_data = NULL;

void libuugeneric_set_oom_handler(oom_handler_t handler, void *data)
{
    _oom_handler = handler;
    _oom_data = data;
}

void *umalloc(size_t size)
{
    void *p = malloc(size);

    if (!p)
    {
        if (_oom_handler(_oom_data))
        {
            p = malloc(size);
        }
    }

    if (!p)
    {
        fprintf(stderr, "out of memory error\n");
        utrace_print();
        exit(EC_OOM);
    }

    return p;
}

void *ucalloc(size_t nmemb, size_t size)
{
    void *p = calloc(nmemb, size);

    if (!p)
    {
        if (_oom_handler(_oom_data))
        {
            p = calloc(nmemb, size);
        }
    }

    if (!p)
    {
        fprintf(stderr, "out of memory error\n");
        utrace_print();
        exit(EC_OOM);
    }

    return p;
}

void *urealloc(void *ptr, size_t size)
{
    void *p = realloc(ptr, size);

    if (!p)
    {
        if (_oom_handler(_oom_data))
        {
            p = realloc(ptr, size);
        }
    }

    if (!p)
    {
        fprintf(stderr, "out of memory error\n");
        utrace_print();
        exit(EC_OOM);
    }

    return p;
}

void ufree(void *ptr)
{
    free(ptr);
}
