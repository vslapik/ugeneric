#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "vector.h"
#include "mem.h"
#include "asserts.h"
#include "string_utils.h"

/*
 * Split string to a vector, make copies of substrings,
 * vector is owner of these substrings.
 */
uvector_t *ustring_split(const char *str, const char *sep)
{
    UASSERT_INPUT(str);
    UASSERT_INPUT(sep);

    char *s = strstr(str, sep);
    size_t off = 0;
    size_t sep_len = strlen(sep);
    uvector_t *v = uvector_create();

    while (s)
    {
        if (s == str)
        {
            // Separator found at the very beginning of the string.
            uvector_append(v, G_STR(ustring_dup("")));
            off = sep_len;
        }
        else
        {
            uvector_append(v, G_STR(ustring_ndup(str + off, s - str - off)));
            off = s - str + sep_len;
        }
        s = strstr(str + off, sep);
    }
    if (strlen(str) != (off - 1))
    {
        uvector_append(v, G_STR(ustring_dup(str + off)));
    }

    return v;
}

char *ustring_replace_char(const char *str, char from, char to)
{
    char *t, *out = NULL;

    if (str)
    {
        t = out = ustring_dup(str);
        while (*t)
        {
            if (*t == from)
            {
                *t = to;
            }
            t++;
        }
    }

    return out;
}

/*
 * Analog of POSIX strdup().
 */
char *ustring_dup(const char *str)
{
    UASSERT_INPUT(str);
    char *p = umalloc(strlen(str) + 1);
    strcpy(p, str);
    return p;
}

/*
 * Analog of POSIX strndup().
 */
char *ustring_ndup(const char *str, size_t n)
{
    UASSERT_INPUT(str);
    char *p = umalloc(n + 1);
    strncpy(p, str, n);
    p[n] = '\0';

    return p;
}

static umemchunk_t _vstr_fmt(const char *fmt, va_list ap)
{
    int size = 0;
    char *str = NULL;
    umemchunk_t mem;
    va_list ap_copy;
    va_copy(ap_copy, ap);

    size = vsnprintf(str, size, fmt, ap_copy);
    if (size > 0)
    {
        size++; // '\0'
        str = umalloc(size);
        UASSERT(vsnprintf(str, size, fmt, ap) > 0);
    }
    va_end(ap_copy);

    mem.data = str;
    mem.size = size - 1;
    return mem;
}

/*
 * Something similar to glibc asprintf.
 */
char *ustring_fmt(const char *fmt, ...)
{
    UASSERT_INPUT(fmt);

    va_list ap, ap_copy;
    umemchunk_t mem;
    char *str = NULL;

    va_start(ap, fmt);
    va_copy(ap_copy, ap);
    mem = _vstr_fmt(fmt, ap_copy);
    va_end(ap);
    va_end(ap_copy);
    str = mem.data;
    str[mem.size] = 0; // room for 0 is assured by _vstr_fmt

    return str;
}

char *ustring_fmt_sized(const char *fmt, size_t *output_size, ...)
{
    UASSERT_INPUT(fmt);

    va_list ap, ap_copy;
    umemchunk_t mem;

    va_start(ap, output_size);
    va_copy(ap_copy, ap);
    mem = _vstr_fmt(fmt, ap_copy);
    va_end(ap);
    va_end(ap_copy);

    if (output_size)
    {
        *output_size = mem.size;
    }

    return mem.data;
}

bool ustring_starts_with(const char *str, const char *prefix)
{
    UASSERT_INPUT(str);
    UASSERT_INPUT(prefix);

    while (*prefix)
    {
        if (*prefix++ != *str++)
        {
            return false;
        }
    }

    return true;
}
