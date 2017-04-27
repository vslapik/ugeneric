#include <string.h>
#include <time.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include "generic.h"
#include "mem.h"
#include "vector.h"
#include "dict.h"
#include "string_utils.h"
#include "buffer.h"

static generic_t _parse_item(const char **str);

static inline void _skip_whitespaces(const char **str)
{
    while (isspace(**str))
    {
        (*str)++;
    }
}

void generic_swap(generic_t *g1, generic_t *g2)
{
    ASSERT_INPUT(g1);
    ASSERT_INPUT(g2);

    generic_t t = *g2;
    *g2 = *g1;
    *g1 = t;
}

int generic_compare(generic_t g1, generic_t g2, void_cmp_t cmp)
{
    if (G_IS_ERROR(g1) || G_IS_ERROR(g2))
    {
        ABORT("attempt to compare G_ERROR object");
    }

    // Generics of different types are always not equal.
    int ret = (generic_get_type(g1) - generic_get_type(g2));
    double f1, f2;

    if (ret == 0)
    {
        switch (generic_get_type(g1))
        {
            case G_NULL_T:
                ret = 0;
                break;

            case G_PTR_T:
                ASSERT_MSG(cmp, "don't know how to compare void pointers");
                ret = cmp(G_AS_PTR(g1), G_AS_PTR(g2));
                break;

            case G_INT_T:
                ret = G_AS_INT(g1) - G_AS_INT(g2);
                break;

            case G_REAL_T:
                f1 = G_AS_REAL(g1);
                f2 = G_AS_REAL(g2);
                if (f1 == f2)
                {
                    ret = 0;
                }
                else if (f1 > f2)
                {
                    ret = 1;
                }
                else
                {
                    ret = -1;
                }
                break;

            case G_SIZE_T:
                ret = G_AS_SIZE(g1) - G_AS_SIZE(g2);
                break;

            case G_VECTOR_T:
                ret = vector_compare(G_AS_PTR(g1), G_AS_PTR(g2), cmp);
                break;

            case G_DICT_T:
                ASSERT(0); // not yet implemented
                break;

            case G_MEMCHUNK_T:
                ASSERT(G_AS_MEMCHUNK_SIZE(g1) == G_AS_MEMCHUNK_SIZE(g2));
                ret = memcmp(G_AS_MEMCHUNK_DATA(g1), G_AS_MEMCHUNK_DATA(g2),
                             G_AS_MEMCHUNK_SIZE(g1));
                break;

            case G_STR_T:
            case G_CSTR_T:
                ret = strcmp(G_AS_STR(g1), G_AS_STR(g2));
                break;

            default:
                ASSERT(0);
        }
    }

    return ret;
}

void generic_destroy(generic_t g, void_dtr_t dtr)
{
    switch (generic_get_type(g))
    {
        case G_PTR_T:
            ASSERT_MSG(dtr, "don't know how to destroy void pointer");
            dtr(G_AS_PTR(g));
            break;

        case G_NULL_T:
        case G_INT_T:
        case G_REAL_T:
        case G_SIZE_T:
        case G_BOOL_T:
        case G_CSTR_T:
            // nothing to be done there
            break;

        case G_VECTOR_T:
            vector_destroy(G_AS_PTR(g));
            break;

        case G_DICT_T:
            dict_destroy(G_AS_PTR(g));
            break;

        case G_MEMCHUNK_T:
            ufree(G_AS_MEMCHUNK_DATA(g));
            break;

        case G_STR_T:
            ufree(G_AS_STR(g));
            break;

        case G_ERROR_T:
            ABORT("attempt to destroy G_ERROR object");
            break;

        default:
            ASSERT(0);
    }
}

void generic_error_print(generic_t g)
{
    ASSERT_INPUT(generic_get_type(g) == G_ERROR_T);
    if (G_AS_STR(g))
    {
        puts(G_AS_STR(g));
    }
    else
    {
        puts("No error message.");
    }
}

void generic_error_destroy(generic_t g)
{
    ASSERT_INPUT(generic_get_type(g) == G_ERROR_T);
    ufree(G_AS_STR(g));
}

generic_t generic_copy(generic_t g, void_cpy_t cpy)
{
    generic_t ret;
    size_t size;
    void *p;

    switch (generic_get_type(g))
    {
        case G_PTR_T:
            ASSERT_MSG(cpy, "don't know how to copy void pointer");
            ret = G_PTR(cpy(G_AS_PTR(g)));
            break;

        case G_NULL_T:
        case G_INT_T:
        case G_REAL_T:
        case G_SIZE_T:
            ret = g;
            break;

        case G_VECTOR_T:
            ret = G_VECTOR(vector_deep_copy(G_AS_PTR(g)));
            break;

        case G_DICT_T:
            ret = G_DICT(dict_deep_copy(G_AS_PTR(g)));
            break;

        case G_MEMCHUNK_T:
            size = G_AS_MEMCHUNK_SIZE(g);
            p = umalloc(size);
            memcpy(p, G_AS_MEMCHUNK_DATA(g), size);
            ret = G_MEMCHUNK(p, size);
            break;

        case G_STR_T:
        case G_CSTR_T:
            ret = G_STR(string_dup(G_AS_STR(g)));
            break;

        case G_ERROR_T:
            ABORT("attempt to copy G_ERROR object");
            break;

        default:
            ASSERT(0);
    }

    return ret;
}

int generic_fprint(generic_t g, void_s8r_t void_serializer, FILE *out)
{
    ASSERT_INPUT(out);

    char *str = generic_as_str(g, void_serializer);
    int ret = fprintf(out, "%s\n", str);
    ufree(str);

    return ret;
}

int generic_print(generic_t g, void_s8r_t void_serializer)
{
    return generic_fprint(g, void_serializer, stdout);
}

char *generic_as_str(generic_t g, void_s8r_t void_serializer)
{
    buffer_t buf = {0};

    generic_serialize(g, &buf, void_serializer);
    buffer_null_terminate(&buf);

    return buf.data;
}

void generic_serialize(generic_t g, buffer_t *buf, void_s8r_t void_serializer)
{
    ASSERT_INPUT(buf);

    char tmp[32];
    memchunk_t m;

    switch (generic_get_type(g))
    {
        case G_NULL_T:
            buffer_append_string(buf, "null");
            break;

        case G_PTR_T:
            if (void_serializer)
            {
                m = void_serializer(G_AS_PTR(g));
                buffer_append_memchunk(buf, &m);
                ufree(m.data);
            }
            else
            {
                snprintf(tmp, sizeof(tmp), "&%p", G_AS_PTR(g));
                m.data = tmp, m.size = strlen(tmp);
                buffer_append_memchunk(buf, &m);
            }
            break;

        case G_STR_T:
        case G_CSTR_T:
            buffer_append_byte(buf, '\"');
            char *s = G_AS_STR(g);
            while (*s)
            {
                if (*s == '"')
                {
                    buffer_append_byte(buf, '\\');
                }
                buffer_append_byte(buf, *s++);
            }
            buffer_append_byte(buf, '\"');
            break;


        case G_INT_T:
            snprintf(tmp, sizeof(tmp), "%ld", G_AS_INT(g));
            m.data = tmp, m.size = strlen(tmp);
            buffer_append_memchunk(buf, &m);
            break;

        case G_REAL_T:
            snprintf(tmp, sizeof(tmp), "%g", G_AS_REAL(g));
            m.data = tmp, m.size = strlen(tmp);
            buffer_append_memchunk(buf, &m);
            break;

        case G_SIZE_T:
            snprintf(tmp, sizeof(tmp), "%zu", G_AS_SIZE(g));
            m.data = tmp, m.size = strlen(tmp);
            buffer_append_memchunk(buf, &m);
            break;

        case G_VECTOR_T:
            vector_serialize(G_AS_PTR(g), buf);
            break;

        case G_DICT_T:
            dict_serialize(G_AS_PTR(g), buf);
            break;

        case G_MEMCHUNK_T:
            memchunk_serialize(G_AS_MEMCHUNK(g), buf);
            break;

        case G_BOOL_T:
            buffer_append_string(buf, G_AS_BOOL(g) ? "true" : "false");
            break;

        case G_ERROR_T:
            ABORT("attempt to serialize G_ERROR object");
            break;

        default:
            ASSERT(0);
    }
}

static generic_t _parse_string(const char **str)
{
    size_t len = 0;
    const char *q = *str + 1;
    char delim = **str;

    // Step over opening quote.
    *str += 1;

    // Count string len skipping escape characters.
    while ((**str != delim) && **str)
    {
        if (**str == '\\')
        {
            *str += 1;
        }
        *str += 1;
        len++;
    }

    if (**str != delim)
    {
        return G_ERROR(string_dup("unexpected end of string"));
    }
    // Step over closing quote.
    *str += 1;

    // Extract the string content.
    char *s = umalloc(len + 1);
    char *t = s;
    while (*q && len)
    {
        if (*q == '\\')
        {
            q++;
        }
        *t++ = *q++;
        len--;
    }
    t[len] = 0;

    return G_STR(s);
}

static generic_t _parse_number(const char **str)
{
    const char *p = *str;
    bool is_real = false;
    generic_t g;

    if (*p == '-')
    {
        p++;
    }
    while (isdigit(*p))
    {
        p++;
    }
    if (*p == '.')
    {
        is_real = true;
    }
    else if (*p == 'e' || *p == 'E')
    {
        is_real = true;
    }

    errno = 0;
    char *endptr = 0;
    if (is_real)
    {
        double r = strtod(*str, &endptr);
        g = G_REAL(r);
    }
    else
    {
        long int l = strtol(*str, &endptr, 10);
        g = G_INT(l);
    }
    if (errno == ERANGE)
    {
        return G_ERROR(string_fmt("%s", strerror(errno)));
    }
    if (endptr == *str)
    {
        return G_ERROR(string_fmt("cannot parse the numerical value"));
    }

    *str += (endptr - *str);

    return g;
}

static generic_t _parse_vector(const char **str)
{
    generic_t g;
    vector_t *v = vector_create();

    (*str)++;

    while (**str)
    {
        _skip_whitespaces(str);
        if (**str == ']')
        {
            break;
        }
        if (G_IS_ERROR(g = _parse_item(str)))
        {
            vector_destroy(v);
            return g;
        }
        vector_append(v, g);

        if (**str == ',')
        {
            (*str)++;
        }
        else
        {
            break;
        }
    }

    if (**str != ']')
    {
        g = G_ERROR(string_dup("expected ']' was not found"));
        vector_destroy(v);
        return g;
    }
    (*str)++;

    vector_shrink_to_size(v);
    return G_VECTOR(v);
}

static generic_t _parse_dict(const char **str)
{
    generic_t k, v, g;
    dict_t *d = dict_create();

    (*str)++;

    while (**str)
    {
        _skip_whitespaces(str);
        if (**str == '}')
        {
            break;
        }

        if (G_IS_ERROR(k = _parse_item(str)))
        {
            dict_destroy(d);
            return k;
        }

        if (**str != ':')
        {
            generic_destroy(k, NULL);
            g = G_ERROR(string_dup("expected ':' was not found"));
            dict_destroy(d);
            return g;
        }

        (*str)++;

        if (G_IS_ERROR(v = _parse_item(str)))
        {
            generic_destroy(k, NULL);
            dict_destroy(d);
            return v;
        }

        dict_put(d, k, v);
        if (**str == ',')
        {
            (*str)++;
        }
    }

    if (**str != '}')
    {
        g = G_ERROR(string_dup("expected '}' was not found"));
        dict_destroy(d);
        return g;
    }
    (*str)++;

    return G_DICT(d);
}

static generic_t _parse_item(const char **str)
{
    generic_t g;

    _skip_whitespaces(str);

    if (**str == '\"' || **str == '\'')
    {
       g = _parse_string(str);
    }
    else if ((**str >= '0' && **str <= '9') || **str == '-')
    {
        g = _parse_number(str);
    }
    else if (**str == '[')
    {
        g = _parse_vector(str);
    }
    else if (**str == '{')
    {
        g = _parse_dict(str);
    }
    else if (!strncmp(*str, "null", 4))
    {
        *str += 4;
        g = G_NULL;
    }
    else if (!strncmp(*str, "true", 4))
    {
        *str += 4;
        g = G_TRUE;
    }
    else if (!strncmp(*str, "false", 5))
    {
        *str += 5;
        g = G_FALSE;
    }
    else
    {
        return G_ERROR(string_dup("unexpected token"));
    }

    _skip_whitespaces(str);

    return g;
}

generic_t generic_parse(const char *str)
{
    ASSERT_INPUT(str);
    const char *err_msg = "Parsing failed at offset %zu: %s.";

    const char *pos = str;
    generic_t g = _parse_item(&pos);
    if (*pos != 0 && !G_IS_ERROR(g))
    {
        generic_destroy(g, NULL);
        g = G_ERROR(string_fmt(err_msg, pos - str, "unexpected end of text"));
    }
    else if (G_IS_ERROR(g))
    {
        generic_t t = G_ERROR(string_fmt(err_msg, pos - str, G_AS_STR(g)));
        generic_error_destroy(g);
        g = t;
    }

    return g;
}

void generic_array_reverse(generic_t *base, size_t nmembs, size_t l, size_t r)
{
    ASSERT_INPUT(l < nmembs);
    ASSERT_INPUT(r < nmembs);
    ASSERT_INPUT(l <= r); // when l == r do nothing

    while (l < r)
        generic_swap(&base[l++], &base[r--]);
}

bool generic_next_permutation(generic_t *base, size_t nmembs, void_cmp_t cmp)
{
    ASSERT_INPUT(base);

    if (!nmembs)
        return false;

    size_t i = nmembs - 1;
    while (i > 0 && generic_compare(base[i - 1], base[i], cmp) >= 0)
    {
       i--;
    }

    if (i == 0)
    {
        return false;
    }

    size_t j = nmembs - 1;
    while (generic_compare(base[j], base[i - 1], cmp) <= 0)
    {
        j--;
    }

    generic_swap(&base[i - 1], &base[j]);
    generic_array_reverse(base, nmembs, i, nmembs - 1);

    return true;
}

static size_t _bsearch(generic_t base[], size_t l, size_t r,
                       generic_t e, void_cmp_t cmp)
{
    size_t m = l + (r - l) / 2;
    int ret = generic_compare(e, base[m], cmp);

    if ((l == r) && ret)
    {
        return SIZE_MAX;
    }

    if (ret < 0)
    {
        return _bsearch(base, l, (m > 0) ? m - 1 : 0, e, cmp);
    }
    else if (ret > 0)
    {
        return _bsearch(base, (m < r) ? m + 1 : r, r, e, cmp);
    }
    else
    {
        return m;
    }
}

size_t generic_bsearch(generic_t *base, size_t nmembs, generic_t e,
                       void_cmp_t cmp)
{
    ASSERT_INPUT(base);
    ASSERT_INPUT(nmembs < SIZE_MAX);
    return (nmembs) ? _bsearch(base, 0, nmembs - 1, e, cmp) : SIZE_MAX;
}

/*
 * murmur3 hash implementation, credits to Austin Appleby.
 */
static uint32_t _hash(const void *key, int len, uint32_t seed)
{
    #define ROTL32(x, r) ((x) << (r)) | ((x) >> (32 - (r)));

    const uint8_t *data = (const uint8_t*)key;
    const int nblocks = len / 4;
    uint32_t h1 = seed;
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;
    const uint32_t *blocks = (const uint32_t *)(data + 4 * nblocks);

    for (int i = -nblocks; i; i++)
    {
        uint32_t k1 = blocks[i];

        k1 *= c1;
        k1 = ROTL32(k1, 15);
        k1 *= c2;
        h1 ^= k1;
        h1 = ROTL32(h1, 13);
        h1 = 5 * h1 + 0xe6546b64;
    }

    const uint8_t *tail = (const uint8_t *)(data + 4 * nblocks);
    uint32_t k1 = 0;

    switch (len & 3)
    {
        case 3: k1 ^= tail[2] << 16;
        case 2: k1 ^= tail[1] << 8;
        case 1: k1 ^= tail[0];
                k1 *= c1; k1 = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
    };

    h1 ^= len;
    h1 ^= h1 >> 16;
    h1 *= 0x85ebca6b;
    h1 ^= h1 >> 13;
    h1 *= 0xc2b2ae35;
    h1 ^= h1 >> 16;

    return h1;
}

size_t generic_hash(generic_t g, void_hasher_t hasher)
{
    void *data;
    size_t size;

    switch (generic_get_type(g))
    {
        case G_NULL_T:
            return 0;

        case G_PTR_T:
            return hasher(G_AS_PTR(g));

        case G_STR_T:
        case G_CSTR_T:
            data = G_AS_STR(g);
            size = strlen(G_AS_STR(g));
            break;


        case G_INT_T:
            return G_AS_INT(g);

        case G_REAL_T:
            data = &G_AS_REAL(g);
            size = sizeof(G_AS_REAL(g));
            break;

        case G_SIZE_T:
            return G_AS_SIZE(g);

        case G_MEMCHUNK_T:
            data = G_AS_MEMCHUNK_DATA(g);
            size = G_AS_MEMCHUNK_SIZE(g);
            break;

        case G_BOOL_T:
            return G_AS_BOOL(g);

        default:
            ASSERT(0);
    }

    ASSERT(size < INT_MAX);
    return _hash(data, size, 0xbaadf00d);
}


/*
 * Generate random number in range [l, h].
 * TODO: quality of this randomness is an open question.
 */
int random_from_range(int l, int h)
{
    ASSERT_INPUT(l < h);
    ASSERT_INPUT(h < RAND_MAX);

    static bool rand_is_initialized = false;

    if (!rand_is_initialized)
    {
        srand((unsigned int)time(NULL) * (uintptr_t)&l);
        rand_is_initialized = true;
    }

    return l + rand() / ((double)RAND_MAX / (h - l + 1));
}
