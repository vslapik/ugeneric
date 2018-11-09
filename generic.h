#ifndef UGENERIC_H__
#define UGENERIC_H__

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "asserts.h"
#include "mem.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    } t;
    union {
        char *err;
        void *ptr;
        char *str;
        const char *cstr;
        long integer;
        double real;
        size_t size;
        bool boolean;
    } v;
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
    return (g.t.type >= G_MEMCHUNK_T) ? G_MEMCHUNK_T : g.t.type;
}

static inline ugeneric_t G_ERROR(char *v)      {ugeneric_t g; g.t.type = G_ERROR_T;  g.v.err = v;         return g;}
static inline ugeneric_t G_PTR(void *v)        {ugeneric_t g; g.t.type = G_PTR_T;    g.v.ptr = v;         return g;}
static inline ugeneric_t G_STR(char *v)        {ugeneric_t g; g.t.type = G_STR_T;    g.v.str = v;         return g;}
static inline ugeneric_t G_CSTR(const char *v) {ugeneric_t g; g.t.type = G_CSTR_T;   g.v.cstr = v;        return g;}
static inline ugeneric_t G_INT(long v)         {ugeneric_t g; g.t.type = G_INT_T;    g.v.integer = v;     return g;}
static inline ugeneric_t G_REAL(double v)      {ugeneric_t g; g.t.type = G_REAL_T;   g.v.real = v;        return g;}
static inline ugeneric_t G_SIZE(size_t v)      {ugeneric_t g; g.t.type = G_SIZE_T;   g.v.size = v;        return g;}
static inline ugeneric_t G_VECTOR(void *v)     {ugeneric_t g; g.t.type = G_VECTOR_T; g.v.ptr = v;         return g;}
static inline ugeneric_t G_DICT(void *v)       {ugeneric_t g; g.t.type = G_DICT_T;   g.v.ptr = v;         return g;}
static inline ugeneric_t G_BOOL(bool v)        {ugeneric_t g; g.t.type = G_BOOL_T;   g.v.boolean = v;     return g;}
static inline ugeneric_t G_NULL(void)          {ugeneric_t g; g.t.type = G_NULL_T;   g.v.integer = 0;     return g;}
static inline ugeneric_t G_TRUE(void)          {ugeneric_t g; g.t.type = G_BOOL_T;   g.v.boolean = true;  return g;}
static inline ugeneric_t G_FALSE(void)         {ugeneric_t g; g.t.type = G_BOOL_T;   g.v.boolean = false; return g;}

#define G_AS_INT(g)    ((g).v.integer)
#define G_AS_REAL(g)   ((g).v.real)
#define G_AS_PTR(g)    ((g).v.ptr)
#define G_AS_SIZE(g)   ((g).v.size)
#define G_AS_STR(g)    ((g).v.str)
#define G_AS_BOOL(g)   ((g).v.boolean)

#define G_AS_MEMCHUNK_DATA(g) ((g).v.ptr)
#define G_AS_MEMCHUNK_SIZE(g) ((g).t.memchunk_size - G_MEMCHUNK_T)

static inline ugeneric_t G_MEMCHUNK(void *ptr, size_t size)
{
    ugeneric_t g;
    g.t.memchunk_size = size + G_MEMCHUNK_T;
    g.v.ptr = ptr;
    return g;
}

static inline umemchunk_t G_AS_MEMCHUNK(ugeneric_t g)
{
    umemchunk_t m;
    m.data = G_AS_MEMCHUNK_DATA(g),
    m.size = G_AS_MEMCHUNK_SIZE(g);
    return m;
}

static inline bool G_IS_ERROR(ugeneric_t g)   {return g.t.type == G_ERROR_T;}
static inline bool G_IS_NULL(ugeneric_t g)    {return g.t.type == G_NULL_T;}
static inline bool G_IS_PTR(ugeneric_t g)     {return g.t.type == G_PTR_T;}
static inline bool G_IS_STR(ugeneric_t g)     {return g.t.type == G_STR_T;}
static inline bool G_IS_CSTR(ugeneric_t g)    {return g.t.type == G_CSTR_T;}
static inline bool G_IS_STRING(ugeneric_t g)  {return g.t.type == G_STR_T || g.t.type == G_CSTR_T;}
static inline bool G_IS_INT(ugeneric_t g)     {return g.t.type == G_INT_T;}
static inline bool G_IS_REAL(ugeneric_t g)    {return g.t.type == G_REAL_T;}
static inline bool G_IS_SIZE(ugeneric_t g)    {return g.t.type == G_SIZE_T;}
static inline bool G_IS_BOOL(ugeneric_t g)    {return g.t.type == G_BOOL_T;}
static inline bool G_IS_TRUE(ugeneric_t g)    {return (g.t.type == G_BOOL_T) && G_AS_BOOL(g);}
static inline bool G_IS_FALSE(ugeneric_t g)   {return (g.t.type == G_BOOL_T) && !G_AS_BOOL(g);}
static inline bool G_IS_VECTOR(ugeneric_t g)  {return g.t.type == G_VECTOR_T;}
static inline bool G_IS_DICT(ugeneric_t g)    {return g.t.type == G_DICT_T;}
static inline bool G_IS_MEMCHUNK(ugeneric_t g){return g.t.type > G_MEMCHUNK_T;}

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

size_t ugeneric_hash(ugeneric_t g, void_hasher_t hasher);
ugeneric_t ugeneric_copy_v(ugeneric_t g, void_cpy_t cpy);
int ugeneric_compare_v(ugeneric_t g1, ugeneric_t g2, void_cmp_t cmp);
void ugeneric_destroy_v(ugeneric_t g, void_dtr_t dtr);

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

#define ugeneric_copy(g) ugeneric_copy_v(g, NULL)
#define ugeneric_compare(g1, g2) ugeneric_compare_v(g1, g2, NULL)
#define ugeneric_destroy(g) ugeneric_destroy_v(g, NULL);

#define ugeneric_serialize(g, buf)           ugeneric_serialize_v(g, buf, NULL)
#define ugeneric_as_str(g)                   ugeneric_as_str_v(g, NULL)
#define ugeneric_print_type(g)               ugeneric_fprint_type(g, stdout)
#define ugeneric_print_v(g, void_serializer) ugeneric_fprint_v(g, stdout, void_serializer)
#define ugeneric_print(g)                    ugeneric_print_v(g, NULL)
#define ugeneric_fprint(g, out)              ugeneric_fprint_v(g, out, NULL)


typedef enum {
    UDICT_KEYS,
    UDICT_VALUES,
    UDICT_KV,
} udict_items_kind_t;

typedef enum {
    UCOPY_SHALLOW,
    UCOPY_DEEP,
} ucopy_kind_t;

// MAX and MIN are not side-effect free, be cautious.
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define ARRAY_LEN(a) (sizeof(a)/sizeof((a)[0]))
#define SCALE_FACTOR 1.5

typedef struct {
    void_cpy_t cpy;
    void_cmp_t cmp;
    void_dtr_t dtr;
    void_s8r_t s8r;
} uvoid_handlers_t;

typedef struct {
    uvoid_handlers_t void_handlers;
    bool is_data_owner;
} ugeneric_base_t;

#define DEFINE_BASE_FUNCS(_ctn_) \
static inline void _ctn_##_set_void_destroyer(_ctn_##_t *self, void_dtr_t dtr)  {_ctn_##_get_base(self)->void_handlers.dtr = dtr;} \
static inline void _ctn_##_set_void_comparator(_ctn_##_t *self, void_cmp_t cmp) {_ctn_##_get_base(self)->void_handlers.cmp = cmp;} \
static inline void _ctn_##_set_void_copier(_ctn_##_t *self, void_cpy_t cpy)     {_ctn_##_get_base(self)->void_handlers.cpy = cpy;} \
static inline void _ctn_##_set_void_serializer(_ctn_##_t *self, void_s8r_t s8r) {_ctn_##_get_base(self)->void_handlers.s8r = s8r;} \
static inline void_dtr_t _ctn_##_get_void_destroyer(_ctn_##_t *self)   {return _ctn_##_get_base(self)->void_handlers.dtr;} \
static inline void_cmp_t _ctn_##_get_void_comparator(_ctn_##_t *self)  {return _ctn_##_get_base(self)->void_handlers.cmp;} \
static inline void_cpy_t _ctn_##_get_void_copier(_ctn_##_t *self)      {return _ctn_##_get_base(self)->void_handlers.cpy;} \
static inline void_s8r_t _ctn_##_get_void_serializer(_ctn_##_t *self)  {return _ctn_##_get_base(self)->void_handlers.s8r;} \
static inline void _ctn_##_take_data_ownership(_ctn_##_t *self) {_ctn_##_get_base(self)->is_data_owner = true;}  \
static inline void _ctn_##_drop_data_ownership(_ctn_##_t *self) {_ctn_##_get_base(self)->is_data_owner = false;} \
static inline bool _ctn_##_is_data_owner(_ctn_##_t *self)       {return _ctn_##_get_base(self)->is_data_owner;}  \

#endif
