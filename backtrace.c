#include <stdio.h>
#include <stdlib.h>

#define DEPTH 128

void print_trace(void)
{
#ifdef __GNU_LIBRARY__
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
        fprintf(stderr, "%s\n", strings[i]);
    }

    free(strings);
#else
    fprintf(stderr, "backtrace is not available");
#endif
}
