#ifndef UASSERTS_H__
#define UASSERTS_H__

#include "backtrace.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef ENABLE_UASSERT_INPUT
    #define UASSERT_INPUT(cond) do {                                        \
        if (!(cond))                                                        \
        {                                                                   \
            fprintf(stderr, "Assertion \"%s\" at %s:%d failed.\n",          \
                    #cond, __FILE__, __LINE__);                             \
            utrace_print();                                                 \
            abort();                                                        \
        }                                                                   \
    } while (0)
#else
    #define UASSERT_INPUT(cond) (void)(cond)
#endif

#ifndef DISABLE_UASSERT_INTERNAL
    #define UASSERT_INTERNAL(cond) do {                                     \
        if (!(cond))                                                        \
        {                                                                   \
            fprintf(stderr, "Sanity violation at %s:%d, "                   \
                            "the \"impossible\" happened.\n",               \
                    __FILE__, __LINE__);                                    \
            utrace_print();                                                 \
            abort();                                                        \
        }                                                                   \
    } while (0)
#else
    #define UASSERT_INTERNAL(cond) (void)(cond)
#endif

#define UASSERT(cond) do {                                                  \
    if (!(cond))                                                            \
    {                                                                       \
        fprintf(stderr, "Assertion \"%s\" at %s:%d failed.\n",              \
                #cond, __FILE__, __LINE__);                                 \
        utrace_print();                                                     \
        abort();                                                            \
    }                                                                       \
} while (0)

#define UASSERT_MSG(cond, msg) do {                                         \
    if (!(cond))                                                            \
    {                                                                       \
        fprintf(stderr, "Error: \"%s\" at %s:%d.\n",                        \
                msg, __FILE__, __LINE__);                                   \
        utrace_print();                                                     \
        abort();                                                            \
    }                                                                       \
} while (0)

#define UASSERT_PERROR(cond) do {                                           \
    if (!(cond))                                                            \
    {                                                                       \
        const char *msg = "Assertion \"%s\" at %s:%d failed: %s\n";         \
        fprintf(stderr, msg, #cond, __FILE__, __LINE__, strerror(errno));   \
        utrace_print();                                                     \
        abort();                                                            \
    }                                                                       \
} while (0)

#define UABORT(msg) do {                                                    \
    fprintf(stderr, "%s:%d %s.\n", __FILE__, __LINE__, msg);                \
    utrace_print();                                                         \
    abort();                                                                \
} while (0)

#define UASSERT_NO_ERROR(g) do {                                            \
    if (G_IS_ERROR(g))                                                      \
    {                                                                       \
        fprintf(stderr, "%s:%d: %s.\n", __FILE__, __LINE__, G_AS_STR(g));   \
        ufree(G_AS_STR(g));                                                 \
        utrace_print();                                                     \
        abort();                                                            \
    }                                                                       \
} while (0)

#define EXIT_IF_ERROR(g, ec) do {                                           \
    if (G_IS_ERROR(g))                                                      \
    {                                                                       \
        fprintf(stderr, "%s:%d: %s.\n", __FILE__, __LINE__, G_AS_STR(g));   \
        ufree(G_AS_STR(g));                                                 \
        utrace_print();                                                     \
        exit(ec);                                                           \
    }                                                                       \
} while (0)


#endif
