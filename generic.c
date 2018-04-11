#include <string.h>
#include <time.h>
#include <ctype.h>
#include <limits.h>
#include <inttypes.h>
#include <errno.h>
#include "generic.h"
#include "mem.h"
#include "vector.h"
#include "dict.h"
#include "string_utils.h"

static ugeneric_t _parse_item(const char **str);

static inline void _skip_whitespaces(const char **str)
{
    while (isspace(**str))
    {
        (*str)++;
    }
}

int ugeneric_compare(ugeneric_t g1, ugeneric_t g2, void_cmp_t cmp)
{
    double f1, f2;
    size_t s1, s2;
    int ret = 0;

    if (G_IS_ERROR(g1) || G_IS_ERROR(g2))
    {
        UABORT("attempt to compare G_ERROR object");
    }

    // Generics of different types are always not equal except G_STR and G_CSTR.
    if (!(G_IS_STRING(g1) && G_IS_STRING(g2)))
    {
        ret = (ugeneric_get_type(g1) - ugeneric_get_type(g2));
    }

    if (ret == 0)
    {
        switch (g1.type)
        {
            case G_NULL_T:
                ret = 0;
                break;

            case G_PTR_T:
                UASSERT_MSG(cmp, "don't know how to compare void data");
                ret = cmp(G_AS_PTR(g1), G_AS_PTR(g2));
                break;

            case G_STR_T:
            case G_CSTR_T:
                ret = strcmp(G_AS_STR(g1), G_AS_STR(g2));
                break;

            case G_INT_T:
                ret = G_AS_INT(g1) - G_AS_INT(g2);
                break;

            case G_REAL_T:
                f1 = G_AS_REAL(g1);
                f2 = G_AS_REAL(g2);
                if ((f1 != f1) || (f2 != f2))
                {
                    UABORT("NAN in comparison");
                }
                if (f1 > f2)
                {
                    ret = 1;
                }
                else if (f1 < f2)
                {
                    ret = -1;
                }
                else
                {
                    ret = 0;
                }
                break;

            case G_SIZE_T:
                ret = G_AS_SIZE(g1) - G_AS_SIZE(g2);
                break;

            case G_BOOL_T:
                ret = G_AS_BOOL(g1) - G_AS_BOOL(g2);
                break;

            case G_VECTOR_T:
                ret = uvector_compare(G_AS_PTR(g1), G_AS_PTR(g2), cmp);
                break;

            case G_DICT_T:
                ret = udict_compare(G_AS_PTR(g1), G_AS_PTR(g2), cmp);
                break;

            default: // G_MEMCHUNK_T
                UASSERT_INTERNAL(g1.type >= G_MEMCHUNK_T);
                s1 = G_AS_MEMCHUNK_SIZE(g1);
                s2 = G_AS_MEMCHUNK_SIZE(g2);
                ret = memcmp(G_AS_MEMCHUNK_DATA(g1), G_AS_MEMCHUNK_DATA(g2),
                             MIN(s1, s2));
                if (ret == 0)
                {
                    ret = s1 - s2;
                }
                break;
        }
    }

    return ret;
}

void ugeneric_destroy(ugeneric_t g, void_dtr_t dtr)
{
    double d;

    switch (g.type)
    {
        case G_PTR_T:
            UASSERT_MSG(dtr, "don't know how to destroy void data");
            dtr(G_AS_PTR(g));
            break;

        case G_NULL_T:
        case G_INT_T:
        case G_SIZE_T:
        case G_BOOL_T:
        case G_CSTR_T:
            // nothing to be done there
            break;

        case G_REAL_T:
            d = G_AS_REAL(g);
            if (d != d)
            {
                UABORT("destroying NAN");
            }
            break;

        case G_VECTOR_T:
            uvector_destroy(G_AS_PTR(g));
            break;

        case G_DICT_T:
            udict_destroy(G_AS_PTR(g));
            break;

        case G_STR_T:
            ufree(G_AS_STR(g));
            break;

        case G_ERROR_T:
            UABORT("attempt to destroy G_ERROR object");
            break;

        default: // G_MEMCHUNK_T
            UASSERT_INTERNAL(g.type >= G_MEMCHUNK_T);
            ufree(G_AS_MEMCHUNK_DATA(g));
            break;
    }
}

void ugeneric_error_print(ugeneric_t g)
{
    UASSERT_INPUT(ugeneric_get_type(g) == G_ERROR_T);

    if (G_AS_STR(g))
    {
        fprintf(stderr, "%s\n", G_AS_STR(g));
    }
    else
    {
        fputs("Unknown error.\n", stderr);
    }
}

void ugeneric_error_destroy(ugeneric_t g)
{
    UASSERT_INPUT(ugeneric_get_type(g) == G_ERROR_T);
    ufree(G_AS_STR(g));
}

ugeneric_t ugeneric_copy(ugeneric_t g, void_cpy_t cpy)
{
    ugeneric_t ret;
    size_t size;
    void *data;
    double d;

    switch (g.type)
    {
        case G_PTR_T:
            UASSERT_MSG(cpy, "don't know how to copy void data");
            ret = G_PTR(cpy(G_AS_PTR(g)));
            break;

        case G_NULL_T:
        case G_INT_T:
        case G_SIZE_T:
        case G_BOOL_T:
            ret = g;
            break;

        case G_REAL_T:
            d = G_AS_REAL(g);
            if (d != d)
            {
                UABORT("copying NAN");
            }
            ret = g;
            break;

        case G_VECTOR_T:
            ret = G_VECTOR(uvector_deep_copy(G_AS_PTR(g)));
            break;

        case G_DICT_T:
            ret = G_DICT(udict_deep_copy(G_AS_PTR(g)));
            break;

        case G_STR_T:
        case G_CSTR_T:
            ret = G_STR(ustring_dup(G_AS_STR(g)));
            break;

        case G_ERROR_T:
            UABORT("attempt to copy G_ERROR object");
            break;

        default: // G_MEMCHUNK_T
            UASSERT_INTERNAL(g.type >= G_MEMCHUNK_T);
            size = G_AS_MEMCHUNK_SIZE(g);
            data = G_AS_MEMCHUNK_DATA(g);
            ret = G_MEMCHUNK(umemdup(data, size), size);
            break;
    }

    return ret;
}

int ugeneric_fprint_v(ugeneric_t g, FILE *out, void_s8r_t void_serializer)
{
    UASSERT_INPUT(out);

    char *str = ugeneric_as_str_v(g, void_serializer);
    int ret = fprintf(out, "%s\n", str);
    ufree(str);

    return ret;
}

char *ugeneric_as_str_v(ugeneric_t g, void_s8r_t void_serializer)
{
    ubuffer_t buf = {0};

    ugeneric_serialize_v(g, &buf, void_serializer);
    ubuffer_null_terminate(&buf);

    return buf.data;
}

void ugeneric_serialize_v(ugeneric_t g, ubuffer_t *buf, void_s8r_t void_serializer)
{
    UASSERT_INPUT(buf);

    char tmp[32];
    char *s;
    umemchunk_t m;

    switch (g.type)
    {
        case G_NULL_T:
            ubuffer_append_string(buf, "null");
            break;

        case G_PTR_T:
            if (void_serializer)
            {
                m.data = void_serializer(G_AS_PTR(g), &m.size);
                ubuffer_append_memchunk(buf, &m);
                ufree(m.data);
            }
            else
            {
                snprintf(tmp, sizeof(tmp), "ptr:0x%p", G_AS_PTR(g));
                m.data = tmp, m.size = strlen(tmp);
                ubuffer_append_memchunk(buf, &m);
            }
            break;

        case G_STR_T:
        case G_CSTR_T:
            ubuffer_append_byte(buf, '\"');
            s = G_AS_STR(g);
            while (*s)
            {
                if (*s == '"')
                {
                    ubuffer_append_byte(buf, '\\');
                }
                ubuffer_append_byte(buf, *s++);
            }
            ubuffer_append_byte(buf, '\"');
            break;

        case G_INT_T:
            snprintf(tmp, sizeof(tmp), "%ld", G_AS_INT(g));
            m.data = tmp, m.size = strlen(tmp);
            ubuffer_append_memchunk(buf, &m);
            break;

        case G_REAL_T:
            snprintf(tmp, sizeof(tmp), "%g", G_AS_REAL(g));
            m.data = tmp, m.size = strlen(tmp);
            ubuffer_append_memchunk(buf, &m);
            break;

        case G_SIZE_T:
            snprintf(tmp, sizeof(tmp), "%zu", G_AS_SIZE(g));
            m.data = tmp, m.size = strlen(tmp);
            ubuffer_append_memchunk(buf, &m);
            break;

        case G_VECTOR_T:
            uvector_serialize(G_AS_PTR(g), buf);
            break;

        case G_DICT_T:
            udict_serialize(G_AS_PTR(g), buf);
            break;

        case G_BOOL_T:
            ubuffer_append_string(buf, G_AS_BOOL(g) ? "true" : "false");
            break;

        case G_ERROR_T:
            UABORT("attempt to serialize G_ERROR object");
            break;

        default: // G_MEMCHUNK_T
            UASSERT_INTERNAL(g.type >= G_MEMCHUNK_T);
            umemchunk_serialize(G_AS_MEMCHUNK(g), buf);
            break;
    }
}

static inline int htoi(int x)
{
    return 9 * (x >> 6) + (x & 0x0f);
}

static ugeneric_t _parse_memchunk(const char **str)
{
    const char *p = *str;

    while (isxdigit(*p))
    {
        p++;
    }

    size_t len = p - *str;

    if (len == 0)
    {
        return G_MEMCHUNK(NULL, 0);
    }

    if (len % 2)
    {
        return G_ERROR(ustring_dup("invalid memchunk size"));
    }

    char *m = umalloc(len / 2);
    p = *str;

    for (size_t i = 0; i < len / 2; i++)
    {
        m[i] = 16 * htoi(p[2 * i]) + htoi(p[2 * i + 1]);
    }

    *str += len;
    return G_MEMCHUNK(m, len / 2);
}

static ugeneric_t _parse_string(const char **str)
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
        return G_ERROR(ustring_dup("unexpected end of string"));
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

static ugeneric_t _parse_number(const char **str)
{
    const char *p = *str;
    bool is_real = false;
    ugeneric_t g;

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
        if (errno == ERANGE && **str != '-')
        {
            // Tooken looks like a huge positive integer number
            // which doesn't fit to long int, let's try to parse
            // it as size_t.
            errno = 0;
            endptr = 0;
            uintmax_t t = strtoumax(*str, &endptr, 10);
            g = G_SIZE(t);
            if (errno != ERANGE)
            {
                // Check for the case when size_t and uintmax_t
                // are different in width.
                if (t != (size_t)t)
                {
                    errno = ERANGE;
                }
            }
        }
    }
    if (errno == ERANGE)
    {
        return G_ERROR(ustring_fmt("%s", strerror(errno)));
    }
    if (endptr == *str)
    {
        return G_ERROR(ustring_dup("cannot parse the numerical value"));
    }

    *str += (endptr - *str);

    return g;
}

static ugeneric_t _parse_vector(const char **str)
{
    ugeneric_t g;
    uvector_t *v = uvector_create();

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
            uvector_destroy(v);
            return g;
        }
        uvector_append(v, g);

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
        g = G_ERROR(ustring_dup("expected ']' was not found"));
        uvector_destroy(v);
        return g;
    }
    (*str)++;

    uvector_shrink_to_size(v);
    return G_VECTOR(v);
}

static ugeneric_t _parse_dict(const char **str)
{
    ugeneric_t k, v, g;
    udict_t *d = udict_create();

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
            udict_destroy(d);
            return k;
        }

        if (**str != ':')
        {
            ugeneric_destroy(k, NULL);
            g = G_ERROR(ustring_dup("expected ':' was not found"));
            udict_destroy(d);
            return g;
        }

        (*str)++;

        if (G_IS_ERROR(v = _parse_item(str)))
        {
            ugeneric_destroy(k, NULL);
            udict_destroy(d);
            return v;
        }

        udict_put(d, k, v);
        if (**str == ',')
        {
            (*str)++;
        }
    }

    if (**str != '}')
    {
        g = G_ERROR(ustring_dup("expected '}' was not found"));
        udict_destroy(d);
        return g;
    }
    (*str)++;

    return G_DICT(d);
}

static ugeneric_t _parse_item(const char **str)
{
    ugeneric_t g;

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
        g = G_NULL();
    }
    else if (!strncmp(*str, "true", 4))
    {
        *str += 4;
        g = G_TRUE();
    }
    else if (!strncmp(*str, "false", 5))
    {
        *str += 5;
        g = G_FALSE();
    }
    else if (!strncmp(*str, "mem:", 4))
    {
        *str += 4;
        g = _parse_memchunk(str);
    }
    else
    {
        return G_ERROR(ustring_dup("unexpected token"));
    }

    _skip_whitespaces(str);

    return g;
}

ugeneric_t ugeneric_parse(const char *str)
{
    UASSERT_INPUT(str);
    const char *err_msg = "Parsing failed at offset %zu: %s.";

    const char *pos = str;
    ugeneric_t g = _parse_item(&pos);
    if (*pos != 0 && !G_IS_ERROR(g))
    {
        ugeneric_destroy(g, NULL);
        g = G_ERROR(ustring_fmt(err_msg, pos - str, "unexpected end of text"));
    }
    else if (G_IS_ERROR(g))
    {
        ugeneric_t t = G_ERROR(ustring_fmt(err_msg, pos - str, G_AS_STR(g)));
        ugeneric_error_destroy(g);
        g = t;
    }

    return g;
}

void ugeneric_array_reverse(ugeneric_t *base, size_t nmemb, size_t l, size_t r)
{
    UASSERT_INPUT(l < nmemb);
    UASSERT_INPUT(r < nmemb);
    UASSERT_INPUT(l <= r); // when l == r do nothing

    while (l < r)
    {
        ugeneric_swap(&base[l++], &base[r--]);
    }
}

bool ugeneric_array_is_sorted(ugeneric_t *base, size_t nmemb, void_cmp_t cmp)
{
    if (nmemb > 1)
    {
        for (size_t i = 0; i < nmemb - 1; i++)
        {
            if (ugeneric_compare(base[i], base[i + 1], cmp) > 0)
            {
                return false;
            }
        }
    }
    return true;
}

bool ugeneric_array_next_permutation(ugeneric_t *base, size_t nmemb, void_cmp_t cmp)
{
    if (nmemb > 1)
    {
        UASSERT_INPUT(base);

        size_t i = nmemb - 1;
        while (i > 0 && ugeneric_compare(base[i - 1], base[i], cmp) >= 0)
        {
           i--;
        }

        if (i == 0)
        {
            return false;
        }

        size_t j = nmemb - 1;
        while (ugeneric_compare(base[j], base[i - 1], cmp) <= 0)
        {
            j--;
        }

        ugeneric_swap(&base[i - 1], &base[j]);
        ugeneric_array_reverse(base, nmemb, i, nmemb - 1);

        return true;
    }
    return false;
}

static size_t _bsearch(ugeneric_t base[], size_t l, size_t r,
                       ugeneric_t e, void_cmp_t cmp)
{
    size_t m = l + (r - l) / 2;
    int ret = ugeneric_compare(e, base[m], cmp);

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

size_t ugeneric_array_bsearch(ugeneric_t *base, size_t nmemb, ugeneric_t e,
                              void_cmp_t cmp)
{
    UASSERT_INPUT(base);
    UASSERT_INPUT(nmemb < SIZE_MAX);
    return (nmemb) ? _bsearch(base, 0, nmemb - 1, e, cmp) : SIZE_MAX;
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

size_t ugeneric_hash(ugeneric_t g, void_hasher_t hasher)
{
    void *data;
    size_t size;

    switch (g.type)
    {
        case G_NULL_T:
            return 0;

        case G_PTR_T:
            UASSERT_MSG(hasher, "don't know how to hash void data");
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

        case G_BOOL_T:
            return G_AS_BOOL(g);

        case G_VECTOR_T:
        case G_DICT_T:
            UABORT("object is not hashable");

        default: // G_MEMCHUNK_T
            UASSERT_INTERNAL(g.type >= G_MEMCHUNK_T);
            data = G_AS_MEMCHUNK_DATA(g);
            size = G_AS_MEMCHUNK_SIZE(g);
            break;
    }

    UASSERT(size < INT_MAX);
    return _hash(data, size, 0xbaadf00d);
}

static bool _rand_is_initialized = false;

/*
 * When the thing crashed the seed value should be preserved in core dump.
 */
static unsigned int _rand_seed = 0;

unsigned int ugeneric_random_init(void)
{
    time_t t = time(NULL);
    // &t returns address of stack variable, stack is subject to ASLR
    unsigned int seed = (unsigned int)t * (uintptr_t)&t;
    ugeneric_random_init_with_seed(seed);
    _rand_seed = seed;
    return seed;
}

void ugeneric_random_init_with_seed(unsigned int seed)
{
    _rand_seed = seed;
    srand(seed);
    _rand_is_initialized = true;
}

/*
 * Generate random number from range. Range is inclusive, [l, h],
 * similar to library call rand(), where range is [0, RAND_MAX].
 */
int ugeneric_random_from_range(int l, int h)
{
    UASSERT_INPUT(l <= h);
    UASSERT_INPUT(h <= RAND_MAX);

    if (!_rand_is_initialized)
    {
        ugeneric_random_init();
    }

    /* TODO: quality of this randomness is an open question. */
    return l + rand() / ((double)RAND_MAX / (h - l + 1));
}
