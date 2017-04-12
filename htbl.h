#ifndef HTBL_H__
#define HTBL_H__

#include "generic.h"

typedef struct htbl_opaq htbl_t;
typedef struct htbl_iterator_opaq htbl_iterator_t;

htbl_t *htbl_create(void);
void htbl_set_destroyer(htbl_t *h, void_dtr_t dtr);
void htbl_set_comparator(htbl_t *h, void_cmp_t cmp);
void htbl_set_hasher(htbl_t *h, void_hasher_t hasher);
void htbl_take_data_ownership(htbl_t *h);
void htbl_drop_data_ownership(htbl_t *h);
void htbl_destroy(htbl_t *h);
void htbl_clear(htbl_t *h);
void htbl_put(htbl_t *h, generic_t k, generic_t v);
generic_t htbl_get(const htbl_t *h, generic_t k, generic_t vdef);
generic_t htbl_pop(htbl_t *h, generic_t k, generic_t vdef);
bool htbl_has_key(const htbl_t *h, generic_t k);
size_t htbl_get_size(const htbl_t *h);
bool htbl_is_empty(const htbl_t *h);

char *htbl_as_str(const htbl_t *h);
void htbl_serialize(const htbl_t *h, buffer_t *buf);
int htbl_fprint(const htbl_t *h, FILE *out);
int htbl_print(const htbl_t *h);

void htbl_dump_to_dot(const htbl_t *h, FILE *out);

htbl_iterator_t *htbl_iterator_create(const htbl_t *h);
generic_kv_t htbl_iterator_get_next(htbl_iterator_t *hi);
bool htbl_iterator_has_next(const htbl_iterator_t *hi);
void htbl_iterator_reset(htbl_iterator_t *hi);
void htbl_iterator_destroy(htbl_iterator_t *hi);

#endif
