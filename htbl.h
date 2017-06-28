#ifndef UHTBL_H__
#define UHTBL_H__

#include "generic.h"
#include "vector.h"

typedef struct uhtbl_opaq uhtbl_t;
typedef struct uhtbl_iterator_opaq uhtbl_iterator_t;

uhtbl_t *uhtbl_create(void);
void uhtbl_set_destroyer(uhtbl_t *h, void_dtr_t dtr);
void uhtbl_set_comparator(uhtbl_t *h, void_cmp_t cmp);
void uhtbl_set_copier(uhtbl_t *h, void_cpy_t cpy);
void uhtbl_set_key_comparator(uhtbl_t *h, void_cmp_t cmp);
void uhtbl_set_hasher(uhtbl_t *h, void_hasher_t hasher);
void uhtbl_take_data_ownership(uhtbl_t *h);
void uhtbl_drop_data_ownership(uhtbl_t *h);
void uhtbl_destroy(uhtbl_t *h);
void uhtbl_clear(uhtbl_t *h);
int uhtbl_compare(const uhtbl_t *h1, const uhtbl_t *h2, void_cmp_t cmp);
void uhtbl_put(uhtbl_t *h, ugeneric_t k, ugeneric_t v);
ugeneric_t uhtbl_get(const uhtbl_t *h, ugeneric_t k, ugeneric_t vdef);
ugeneric_t uhtbl_pop(uhtbl_t *h, ugeneric_t k, ugeneric_t vdef);
bool uhtbl_has_key(const uhtbl_t *h, ugeneric_t k);
size_t uhtbl_get_size(const uhtbl_t *h);
bool uhtbl_is_empty(const uhtbl_t *h);

char *uhtbl_as_str(const uhtbl_t *h);
void uhtbl_serialize(const uhtbl_t *h, ubuffer_t *buf);
int uhtbl_fprint(const uhtbl_t *h, FILE *out);
int uhtbl_print(const uhtbl_t *h);

void uhtbl_dump_to_dot(const uhtbl_t *h, FILE *out);

uhtbl_iterator_t *uhtbl_iterator_create(const uhtbl_t *h);
ugeneric_kv_t uhtbl_iterator_get_next(uhtbl_iterator_t *hi);
bool uhtbl_iterator_has_next(const uhtbl_iterator_t *hi);
void uhtbl_iterator_reset(uhtbl_iterator_t *hi);
void uhtbl_iterator_destroy(uhtbl_iterator_t *hi);

uvector_t *uhtbl_get_items(const uhtbl_t *h, udict_items_kind_t kind);
static inline uvector_t *uhtbl_get_keys(const uhtbl_t *h) { return uhtbl_get_items(h, UDICT_KEYS); }
static inline uvector_t *uhtbl_get_values(const uhtbl_t *h) { return uhtbl_get_items(h, UDICT_VALUES); }

#endif
