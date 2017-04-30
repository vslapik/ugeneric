#ifndef UT_UTILS_H__
#define UT_UTILS_H__

#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <errno.h>
#include <string.h>

#include "backtrace.h"

#define UASSERT_STR_EQ(s1, s2) do {                                          \
    const char *__s1 = (s1); if (!__s1) __s1 = "null";                      \
    const char *__s2 = (s2); if (!__s2) __s2 = "null";                      \
    if (strcmp(__s1, __s2) != 0)                                            \
    {                                                                       \
        fprintf(stderr, "Assertion failed at %s:%d: \"%s\" != \"%s\".\n",   \
                __FILE__, __LINE__, __s1, __s2);                            \
        utrace_print();                                                     \
        abort();                                                            \
    }                                                                       \
} while (0)

#define UASSERT_INT_EQ(i1, i2) do {                                          \
    long __i1 = (i1);                                                       \
    long __i2 = (i2);                                                       \
    if (__i1 != __i2)                                                       \
    {                                                                       \
        fprintf(stderr, "Assertion failed at %s:%d: %ld != %ld.\n",         \
                __FILE__, __LINE__, __i1, __i2);                            \
        utrace_print();                                                     \
        abort();                                                            \
    }                                                                       \
} while (0)

#define UASSERT_SIZE_EQ(sz1, sz2) do {                                       \
    size_t __sz1 = (sz1);                                                   \
    size_t __sz2 = (sz2);                                                   \
    if (__sz1 != __sz2)                                                     \
    {                                                                       \
        fprintf(stderr, "Assertion failed at %s:%d: %zd != %zd.\n",         \
                __FILE__, __LINE__, __sz1, __sz2);                          \
        utrace_print();                                                     \
        abort();                                                            \
    }                                                                       \
} while (0)

#define UASSERT_UABORTS(s) do {                                               \
    pid_t pid = fork();                                                     \
    if (pid == 0)                                                           \
    {                                                                       \
        /* child */                                                         \
        struct rlimit core_limit;                                           \
        core_limit.rlim_cur = 0;                                            \
        core_limit.rlim_max = 0;                                            \
                                                                            \
        if (setrlimit(RLIMIT_CORE, &core_limit) < 0)                        \
        {                                                                   \
            fprintf(stderr,                                                 \
                    "setrlimit: Can not disable core dumps generation in child process: %s.\n", \
                     strerror(errno));                                      \
        }                                                                   \
        else                                                                \
        {                                                                   \
            fclose(stdin);                                                  \
            fclose(stdout);                                                 \
            fclose(stderr);                                                 \
            s;                                                              \
        }                                                                   \
         return EXIT_SUCCESS;                                               \
    }                                                                       \
    else if (pid > 0)                                                       \
    {                                                                       \
        /* parent */                                                        \
        int status;                                                         \
        if (waitpid(pid, &status, 0) != pid)                                \
        {                                                                   \
            printf("Can not happen.");                                      \
            return EXIT_FAILURE;                                            \
        }                                                                   \
        else                                                                \
        {                                                                   \
            if (!WIFSIGNALED(status) ||                                     \
                WTERMSIG(status) != 6)                                      \
            {                                                               \
                printf("Expected UABORT at %s:%u didn't happen.\n",          \
                        __FILE__, __LINE__);                                \
                return EXIT_FAILURE;                                        \
            }                                                               \
        }                                                                   \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        /* fork failed */                                                   \
        printf("fork() failed!\n");                                         \
        return EXIT_FAILURE;                                                \
    }                                                                       \
} while (0)

#endif
