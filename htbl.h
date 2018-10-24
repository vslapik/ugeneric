#ifndef UHTBL_H__
#define UHTBL_H__

#include "generic.h"
#include "vector.h"

typedef enum {
    UHTBL_TYPE_DEFAULT,
    UHTBL_TYPE_CHAINING,
    UHTBL_TYPE_OPEN_ADDRESSING,
    UHTBL_TYPE_MAX, // keep it last
} uhtbl_type_t;

typedef struct uhtbl_opaq uhtbl_t;
typedef struct uhtbl_iterator_opaq uhtbl_iterator_t;

uhtbl_t *uhtbl_create(void);
uhtbl_t *uhtbl_create_with_type(uhtbl_type_t type);
void uhtbl_set_void_key_comparator(uhtbl_t *h, void_cmp_t cmp);
void_cmp_t uhtbl_get_void_key_comparator(const uhtbl_t *h);
void uhtbl_set_void_hasher(uhtbl_t *h, void_hasher_t hasher);
void_hasher_t uhtbl_get_void_hasher(const uhtbl_t *h);

static bool uhtbl_is_data_owner(uhtbl_t *h);
static void uhtbl_take_data_ownership(uhtbl_t *h);
static void uhtbl_drop_data_ownership(uhtbl_t *h);

void uhtbl_destroy(uhtbl_t *h);
void uhtbl_clear(uhtbl_t *h);
void uhtbl_put(uhtbl_t *h, ugeneric_t k, ugeneric_t v);
ugeneric_t uhtbl_get(const uhtbl_t *h, ugeneric_t k, ugeneric_t vdef);
ugeneric_t uhtbl_pop(uhtbl_t *h, ugeneric_t k, ugeneric_t vdef);
bool uhtbl_remove(uhtbl_t *h, ugeneric_t k);
bool uhtbl_has_key(const uhtbl_t *h, ugeneric_t k);
size_t uhtbl_get_size(const uhtbl_t *h);
bool uhtbl_is_empty(const uhtbl_t *h);

char *uhtbl_as_str(const uhtbl_t *h);
void uhtbl_serialize(const uhtbl_t *h, ubuffer_t *buf);
int uhtbl_fprint(const uhtbl_t *h, FILE *out);
static inline int uhtbl_print(const uhtbl_t *h) {return uhtbl_fprint(h, stdout);}

void uhtbl_dump_to_dot(const uhtbl_t *h, const char *name, FILE *out);

uhtbl_iterator_t *uhtbl_iterator_create(const uhtbl_t *h);
ugeneric_kv_t uhtbl_iterator_get_next(uhtbl_iterator_t *hi);
bool uhtbl_iterator_has_next(const uhtbl_iterator_t *hi);
void uhtbl_iterator_reset(uhtbl_iterator_t *hi);
void uhtbl_iterator_destroy(uhtbl_iterator_t *hi);

uvector_t *uhtbl_get_items(const uhtbl_t *h, udict_items_kind_t kind, bool deep);
static inline uvector_t *uhtbl_get_keys(const uhtbl_t *h, bool deep) {return uhtbl_get_items(h, UDICT_KEYS, deep);}
static inline uvector_t *uhtbl_get_values(const uhtbl_t *h, bool deep) {return uhtbl_get_items(h, UDICT_VALUES, deep);}

ugeneric_base_t *uhtbl_get_base(uhtbl_t *h);
DEFINE_BASE_FUNCS(uhtbl)

#endif
