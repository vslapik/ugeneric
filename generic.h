#ifndef UGENERIC_H__
#define UGENERIC_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>

#include "mem.h"
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
    G_DICT_T    = 10,   // Associative array of generics.

    /*
     * G_MEMCHUNK_T should be the last in the list, values greater than
     * G_MEMCHUNK_T represent size of mchunk. Value of G_MEMCHUNK_T
     * essentially represents memory chunk of exactly 0 size.
     */
    G_MEMCHUNK_T = 11,  // Reference to a chunk of memory.
} ugeneric_type_e;

typedef struct {
    /*
     * Enum type is int in C, we want size_t, that's why we need union here.
     */
    union {
        size_t memchunk_size;
        ugeneric_type_e type;
    };
    union {
        char *err;
        void *ptr;
        char *str;
        const char *cstr;
        long integer;
        double real;
        size_t size;
        bool boolean;
    };
} ugeneric_t;

typedef struct {
    ugeneric_t k;
    ugeneric_t v;
} ugeneric_kv_t;

typedef enum {
    UGENERIC_EXIT_OOM = 17,
    UGENERIC_EXIT_IO = 18,
    UGENERIC_EXIT_FORMAT = 19,
} ugeneric_exit_code_t;

static inline ugeneric_type_e ugeneric_get_type(ugeneric_t g)
{
    return (g.type >= G_MEMCHUNK_T) ? G_MEMCHUNK_T : g.type;
}

static inline ugeneric_t G_ERROR(char *v) {ugeneric_t g; g.type = G_ERROR_T; g.err = v; return g;}
static inline ugeneric_t G_PTR(void *v)   {ugeneric_t g; g.type = G_PTR_T;   g.ptr = v; return g;}
static inline ugeneric_t G_STR(char *v)   {ugeneric_t g; g.type = G_STR_T;   g.str = v; return g;}
static inline ugeneric_t G_CSTR(const char *v) {ugeneric_t g; g.type = G_CSTR_T;  g.cstr = v; return g;}
static inline ugeneric_t G_INT(long v)    {ugeneric_t g; g.type = G_INT_T;   g.integer = v; return g;}
static inline ugeneric_t G_REAL(double v) {ugeneric_t g; g.type = G_REAL_T;  g.real = v; return g;}
static inline ugeneric_t G_SIZE(size_t v) {ugeneric_t g; g.type = G_SIZE_T;  g.size = v; return g;}
static inline ugeneric_t G_VECTOR(void *v){ugeneric_t g; g.type = G_VECTOR_T; g.ptr = v; return g;}
static inline ugeneric_t G_DICT(void *v)  {ugeneric_t g; g.type = G_DICT_T;  g.ptr = v; return g;}
static inline ugeneric_t G_BOOL(bool v)   {ugeneric_t g; g.type = G_BOOL_T; g.boolean = v; return g;}
static inline ugeneric_t G_NULL(void)     {ugeneric_t g; g.type = G_NULL_T; return g;}
static inline ugeneric_t G_TRUE(void)     {ugeneric_t g; g.type = G_BOOL_T; g.boolean = true; return g;}
static inline ugeneric_t G_FALSE(void)    {ugeneric_t g; g.type = G_BOOL_T; g.boolean = false;  return g;}

#define G_AS_INT(g)    ((g).integer)
#define G_AS_REAL(g)   ((g).real)
#define G_AS_PTR(g)    ((g).ptr)
#define G_AS_SIZE(g)   ((g).size)
#define G_AS_STR(g)    ((g).str)
#define G_AS_BOOL(g)   ((g).boolean)

#define G_AS_MEMCHUNK_DATA(g) ((g).ptr)
#define G_AS_MEMCHUNK_SIZE(g) ((g).memchunk_size - G_MEMCHUNK_T)

static inline ugeneric_t G_MEMCHUNK(void *ptr, size_t size)
{
    ugeneric_t g;
    g.memchunk_size = size + G_MEMCHUNK_T;
    g.ptr = ptr;
    return g;
}

static inline umemchunk_t G_AS_MEMCHUNK(ugeneric_t g)
{
    umemchunk_t m;
    m.data = G_AS_MEMCHUNK_DATA(g),
    m.size = G_AS_MEMCHUNK_SIZE(g);
    return m;
}

static inline bool G_IS_ERROR(ugeneric_t g)   {return g.type == G_ERROR_T;}
static inline bool G_IS_NULL(ugeneric_t g)    {return g.type == G_NULL_T;}
static inline bool G_IS_PTR(ugeneric_t g)     {return g.type == G_PTR_T;}
static inline bool G_IS_STR(ugeneric_t g)     {return g.type == G_STR_T;}
static inline bool G_IS_CSTR(ugeneric_t g)    {return g.type == G_CSTR_T;}
static inline bool G_IS_STRING(ugeneric_t g)  {return G_IS_CSTR(g) || G_IS_STR(g);}
static inline bool G_IS_INT(ugeneric_t g)     {return g.type == G_INT_T;}
static inline bool G_IS_REAL(ugeneric_t g)    {return g.type == G_REAL_T;}
static inline bool G_IS_SIZE(ugeneric_t g)    {return g.type == G_SIZE_T;}
static inline bool G_IS_BOOL(ugeneric_t g)    {return g.type == G_BOOL_T;}
static inline bool G_IS_TRUE(ugeneric_t g)    {return (g.type == G_BOOL_T) && G_AS_BOOL(g);}
static inline bool G_IS_FALSE(ugeneric_t g)   {return (g.type == G_BOOL_T) && !G_AS_BOOL(g);}
static inline bool G_IS_VECTOR(ugeneric_t g)  {return g.type == G_VECTOR_T;}
static inline bool G_IS_DICT(ugeneric_t g)    {return g.type == G_DICT_T;}
static inline bool G_IS_MEMCHUNK(ugeneric_t g){return g.type > G_MEMCHUNK_T;}

static inline void ugeneric_swap(ugeneric_t *g1, ugeneric_t *g2)
{
    ugeneric_t t = *g2;
    *g2 = *g1;
    *g1 = t;
}

typedef int (*void_cmp_t)(const void *ptr1, const void *ptr2);
typedef void *(*void_cpy_t)(const void *ptr);
typedef void (*void_dtr_t)(void *ptr);
typedef char *(*void_s8r_t)(const void *ptr, size_t *output_size);
typedef size_t (*void_hasher_t)(const void *ptr);
typedef bool (*ugeneric_kv_iter_t)(ugeneric_t k, ugeneric_t v, void *data);
typedef void (*ugeneric_sorter_t)(ugeneric_t *base, size_t nmemb, void_cmp_t cmp);

ugeneric_t ugeneric_copy(ugeneric_t g, void_cpy_t cpy);
int ugeneric_compare(ugeneric_t g1, ugeneric_t g2, void_cmp_t cmp);
void ugeneric_destroy(ugeneric_t g, void_dtr_t dtr);
void ugeneric_swap(ugeneric_t *g1, ugeneric_t *g2);
size_t ugeneric_hash(ugeneric_t g, void_hasher_t hasher);

void ugeneric_error_destroy(ugeneric_t g);
void ugeneric_error_print(ugeneric_t g);

ugeneric_t ugeneric_parse(const char *str);

void ugeneric_array_reverse(ugeneric_t *base, size_t nmemb, size_t l, size_t r);
bool ugeneric_array_is_sorted(ugeneric_t *base, size_t nmemb, void_cmp_t cmp);
bool ugeneric_array_next_permutation(ugeneric_t *base, size_t nmemb, void_cmp_t cmp);
size_t ugeneric_array_bsearch(ugeneric_t *base, size_t nmemb, ugeneric_t e,
                              void_cmp_t cmp);
/* PRN */
unsigned int ugeneric_random_init(void);
void ugeneric_random_init_with_seed(unsigned int seed);
int random_from_range(int start, int stop);
int ugeneric_random_from_range(int l, int h);

char *ugeneric_as_str_v(ugeneric_t g, void_s8r_t void_serializer);
void ugeneric_serialize_v(ugeneric_t g, ubuffer_t *buf, void_s8r_t void_serializer);
int ugeneric_fprint_v(ugeneric_t g, FILE *out, void_s8r_t void_serializer);

static inline void ugeneric_serialize(ugeneric_t g, ubuffer_t *buf) {ugeneric_serialize_v(g, buf, NULL);}
static inline char *ugeneric_as_str(ugeneric_t g) {return ugeneric_as_str_v(g, NULL);}
static inline int ugeneric_print_v(ugeneric_t g, void_s8r_t void_serializer) {return ugeneric_fprint_v(g, stdout, void_serializer);}
static inline int ugeneric_print(ugeneric_t g) {return ugeneric_print_v(g, NULL);}
static inline int ugeneric_fprint(ugeneric_t g, FILE *out) {return ugeneric_fprint_v(g, out, NULL);}

typedef enum {
    UDICT_KEYS,
    UDICT_VALUES,
    UDICT_KV,
} udict_items_kind_t;

// MAX and MIN are not side-effect free, be cautious.
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define ARR_LEN(a) sizeof(a)/sizeof(a[0])
#define SCALE_FACTOR 2

#endif
