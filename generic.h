#ifndef GENERIC_H__
#define GENERIC_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "buffer.h"
#include "asserts.h"

typedef enum {
    G_ERROR_T   = 0,    // Error object.
    G_NULL_T    = 1,    // null, None, NULL, nil, whatever ...
    G_PTR_T     = 2,    // Generic data pointer, no size information.
    G_STR_T     = 3,    // Reference to a string.
    G_CSTR_T    = 4,    // Reference to a constant string.
    G_INT_T     = 5,    // Signed integer value (long).
    G_REAL_T    = 6,    // Real value (double).
    G_SIZE_T    = 7,    // Unsigned integer value (size_t).
    G_BOOL_T    = 8,    // Boolean (G_TRUE or G_FALSE).
    G_VECTOR_T  = 9,    // Dynamically resizable array of generics.
    G_DICT_T   = 10,    // Associative array of generics.

    /*
     * G_MCHUNK_T should be the last in the list, values greater than
     * G_MCHUNK_T represent size of mchunk. Value of G_MCHUNK_T
     * essentially represents memory chunk of exactly 0 size.
     */
    G_MCHUNK_T = 11,    // Reference to a chunk of memory.
} generic_type_e;

/*
 * Enum type is int in C, we want size_t, that's why we need union here.
 */
typedef union {
    size_t size;
    generic_type_e type;
} generic_type_t;

typedef union {
    char *err;
    void *ptr;
    char *str;
    const char *cstr;
    long integer;
    double real;
    char chr;
    size_t size;
    bool boolean;
} generic_value_t;

typedef struct {
    generic_type_t type;
    generic_value_t value;
} generic_t;

typedef struct {
    generic_t k;
    generic_t v;
} generic_kv_t;

typedef enum {
    EC_OOM = 17,
    EC_IO = 18,
} exit_code_t;

static inline generic_type_e generic_get_type(generic_t g)
{
    return (g.type.type >= G_MCHUNK_T) ? G_MCHUNK_T : g.type.type;
}

#define G_NULL         ((generic_t){.type.type = G_NULL_T})

#define G_ERROR(v)     ((generic_t){.type.type = G_ERROR_T,            \
                                    .value = {.str = (v)}})
#define G_PTR(v)       ((generic_t){.value = {.ptr = (v)},             \
                                    .type.type = G_PTR_T})
#define G_STR(v)       ((generic_t){.value = {.str = (v)},             \
                                    .type.type = G_STR_T})
#define G_CSTR(v)      ((generic_t){.value = {.cstr = (v)},            \
                                    .type.type = G_CSTR_T})
#define G_INT(v)       ((generic_t){.value = {.integer = (v)},         \
                                    .type.type = G_INT_T})
#define G_REAL(v)      ((generic_t){.value = {.real = (v)},            \
                                    .type.type = G_REAL_T})
#define G_SIZE(v)      ((generic_t){.value = {.size = (v)},            \
                                    .type.type = G_SIZE_T})
#define G_VECTOR(v)    ((generic_t){.value = {.ptr = (v)},             \
                                    .type.type = G_VECTOR_T})
#define G_DICT(v)      ((generic_t){.value = {.ptr = (v)},             \
                                    .type.type = G_DICT_T})

#define G_MCHUNK(p, s) ((generic_t){.value = {.ptr = (p)},             \
                                    .type.size = s + G_MCHUNK_T})
#define G_TRUE         ((generic_t){.type.type = G_BOOL_T,             \
                                    .value = {.boolean = true}})
#define G_FALSE        ((generic_t){.type.type = G_BOOL_T,             \
                                    .value = {.boolean = false}})

#define G_AS_INT(g)    ((g).value.integer)
#define G_AS_REAL(g)   ((g).value.real)
#define G_AS_PTR(g)    ((g).value.ptr)
#define G_AS_SIZE(g)   ((g).value.size)
#define G_AS_STR(g)    ((g).value.str)
#define G_AS_BOOL(g)   ((g).value.boolean)

#define G_AS_MCHUNK_DATA(g) ((g).value.ptr)
#define G_AS_MCHUNK_SIZE(g) ((g).type.size - G_MCHUNK_T)
#define G_AS_MCHUNK(g) ((memchunk_t){.data = G_AS_MCHUNK_DATA(g),      \
                                     .size = G_AS_MCHUNK_SIZE(g)})

static inline bool G_IS_ERROR(generic_t g) {return g.type.type == G_ERROR_T;}
static inline bool G_IS_NULL(generic_t g)  {return g.type.type == G_NULL_T;}
static inline bool G_IS_PTR(generic_t g)   {return g.type.type == G_PTR_T;}
static inline bool G_IS_STR(generic_t g)   {return g.type.type == G_STR_T;}
static inline bool G_IS_CSTR(generic_t g)  {return g.type.type == G_STR_T;}
static inline bool G_IS_STRING(generic_t g){return G_IS_CSTR(g) || G_IS_STR(g);}
static inline bool G_IS_INT(generic_t g)   {return g.type.type == G_INT_T;}
static inline bool G_IS_REAL(generic_t g)  {return g.type.type == G_REAL_T;}
static inline bool G_IS_SIZE(generic_t g)  {return g.type.type == G_SIZE_T;}
static inline bool G_IS_BOOL(generic_t g)  {return g.type.type == G_BOOL_T;}
static inline bool G_IS_TRUE(generic_t g)  {return (g.type.type == G_BOOL_T) && G_AS_BOOL(g);}
static inline bool G_IS_FALSE(generic_t g) {return (g.type.type == G_BOOL_T) && !G_AS_BOOL(g);}
static inline bool G_IS_VECTOR(generic_t g){return g.type.type == G_VECTOR_T;}
static inline bool G_IS_DICT(generic_t g)  {return g.type.type == G_DICT_T;}
static inline bool G_IS_MCHUNK(generic_t g){return g.type.type > G_MCHUNK_T;}

typedef int (*void_cmp_t)(const void *ptr1, const void *ptr2);
typedef void *(*void_cpy_t)(const void *ptr);
typedef void (*void_dtr_t)(void *ptr);
typedef size_t (*void_hasher_t)(const void *data);
typedef bool (*generic_kv_iter_t)(generic_t k, generic_t v, void *data);

generic_t generic_copy(generic_t g, void_cpy_t cpy);
int generic_compare(generic_t g1, generic_t g2, void_cmp_t cmp);
void generic_destroy(generic_t g, void_dtr_t dtr);
void generic_swap(generic_t *g1, generic_t *g2);
size_t generic_hash(generic_t g, void_hasher_t hasher);

void generic_error_destroy(generic_t g);
void generic_error_print(generic_t g);

char *generic_as_str(generic_t g);
void generic_serialize(generic_t g, buffer_t *buf);
int generic_fprint(generic_t g, FILE *out);
int generic_print(generic_t g);

generic_t generic_parse(const char *str);

void generic_array_reverse(generic_t *base, size_t nmembs, size_t l, size_t r);
bool generic_next_permutation(generic_t *base, size_t nmembs, void_cmp_t cmp);
size_t generic_bsearch(generic_t *base, size_t nmembs, generic_t e,
                       void_cmp_t cmp);

int random_from_range(int start, int stop);

// MAX and MIN are not side-effect free, be cautious.
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define ARR_LEN(a) sizeof(a)/sizeof(a[0])

#define SCALE_FACTOR 2

#endif
