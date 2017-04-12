#ifndef ASSERTS_H__
#define ASSERTS_H__

#include <stdio.h>
#include <stdlib.h>
#include "backtrace.h"

#ifdef ENABLE_ASSERT_INPUT
    #define ASSERT_INPUT(cond) do {                                         \
        if (!(cond))                                                        \
        {                                                                   \
            fprintf(stderr, "Assertion \"%s\" at %s:%d failed.\n",          \
                    #cond, __FILE__, __LINE__);                             \
            print_trace();                                                  \
            abort();                                                        \
        }                                                                   \
    } while (0)
#else
    #define ASSERT_INPUT(cond) (void)(cond)
#endif

#define ASSERT(cond) do {                                                   \
    if (!(cond))                                                            \
    {                                                                       \
        fprintf(stderr, "Assertion \"%s\" at %s:%d failed.\n",              \
                #cond, __FILE__, __LINE__);                                 \
        print_trace();                                                      \
        abort();                                                            \
    }                                                                       \
} while (0)

#define ASSERT_MSG(cond, msg) do {                                          \
    if (!(cond))                                                            \
    {                                                                       \
        fprintf(stderr, "Error: \"%s\" at %s:%d.\n",                        \
                msg, __FILE__, __LINE__);                                   \
        print_trace();                                                      \
        abort();                                                            \
    }                                                                       \
} while (0)

#define ABORT(msg) do {                                                     \
    fprintf(stderr, "%s:%d %s.\n", __FILE__, __LINE__, msg);                \
    print_trace();                                                          \
    abort();                                                                \
} while (0)

#define ASSERT_NO_ERROR(g) do {                                             \
    if (G_IS_ERROR(g))                                                      \
    {                                                                       \
        fprintf(stderr, "%s:%d %s\n", __FILE__, __LINE__, G_AS_STR(g));    \
        ufree(G_AS_STR(g));                                                 \
        abort();                                                            \
    }                                                                       \
} while (0)

#endif
