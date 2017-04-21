#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "vector.h"
#include "mem.h"
#include "asserts.h"
#include "string_utils.h"

/*
 * Split string to a vector, makes copies of substrings,
 * vector is owner of these substrings.
 */
vector_t *string_split(const char *str, const char *sep)
{
    ASSERT_INPUT(str);
    ASSERT_INPUT(sep);

    char *s = strstr(str, sep);
    size_t off = 0;
    size_t sep_len = strlen(sep);
    vector_t *v = vector_create();

    while (s)
    {
        if (s == str)
        {
            // Separator found at the very beginning of the string.
            vector_append(v, G_STR(string_dup("")));
            off = sep_len;
        }
        else
        {
            vector_append(v, G_STR(string_ndup(str + off, s - str - off)));
            off = s - str + sep_len;
        }
        s = strstr(str + off, sep);
    }
    if (strlen(str) != (off - 1))
    {
        vector_append(v, G_STR(string_dup(str + off)));
    }

    return v;
}

/*
 * Analog of POSIX strdup().
 */
char *string_dup(const char *str)
{
    ASSERT_INPUT(str);
    char *p = umalloc(strlen(str) + 1);
    strcpy(p, str);

    return p;
}

/*
 * Analog of POSIX strndup().
 */
char *string_ndup(const char *str, size_t n)
{
    ASSERT_INPUT(str);
    char *p = umalloc(n + 1);
    strncpy(p, str, n);
    p[n] = '\0';

    return p;
}

/*
 * Something similar to glibc asprintf.
 */
char *string_fmt(const char *fmt, ...)
{
    ASSERT_INPUT(fmt);

    int size = 0;
    char *str = NULL;
    va_list ap, ap_copy;
    va_start(ap, fmt);
    va_copy(ap_copy, ap);

    size = vsnprintf(str, size, fmt, ap_copy);
    if (size > 0)
    {
        size++; // '\0'
        str = umalloc(size);
        ASSERT(vsnprintf(str, size, fmt, ap) > 0);
    }
    va_end(ap_copy);
    va_end(ap);

    return str;
}

bool string_starts_with(const char *str, const char *prefix)
{
    ASSERT_INPUT(str);
    ASSERT_INPUT(str);

    while (*prefix)
    {
        if (*prefix++ != *str++)
        {
            return false;
        }
    }

    return true;
}
