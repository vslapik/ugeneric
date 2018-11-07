#include "backtrace.h"

#include <stdio.h>
#include <stdlib.h>

#define DEPTH 128

void utrace_print(void)
{
#if defined(__GNU_LIBRARY__) && !defined(__cplusplus)
#include <execinfo.h>
    static void *array[DEPTH];
    size_t size;
    char **strings;

    size = backtrace(array, DEPTH);
    strings = backtrace_symbols(array, size);

    if (size >= DEPTH)
    {
        fprintf(stderr,
                "Can not obtain more than %zu stack frames.\n", size);
    }

    for (size_t i = 1; i < size; i++)
    {
        fputs(strings[i], stderr);
        fputs("\n", stderr);
    }

    free(strings);
#else
    fprintf(stderr, "Backtrace is not available.\n");
#endif
}
