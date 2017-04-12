#ifndef LIST_H__
#define LIST_H__

#include "generic.h"

typedef struct list_opaq list_t;
typedef struct list_iterator_opaq list_iterator_t;

list_t *list_create(void);
void list_take_data_ownership(list_t *l);
void list_drop_data_ownership(list_t *l);
void list_destroy(list_t *l);
void list_append(list_t *l, generic_t e);
void list_prepend(list_t *l, generic_t e);
generic_t list_pop_back(list_t *l);
generic_t list_pop_front(list_t *l);
void list_clear(list_t *l);
bool list_is_empty(const list_t *l);
size_t list_get_size(const list_t *l);
generic_t list_get_at(const list_t *l, size_t i);
void list_set_at(list_t *l, size_t i, generic_t e);
void list_insert_at(list_t *l, size_t i, generic_t e);
void list_remove_at(list_t *l, size_t i);
bool list_contains(const list_t *l, generic_t e);
void list_reverse(list_t *l);
list_t *list_copy(const list_t *l);
list_t *list_deep_copy(const list_t *l);
int list_compare(const list_t *l1, const list_t *l2, void_cmp_t cmp);

char *list_as_str(const list_t *l);
void list_serialize(const list_t *l, buffer_t *buf);
int list_fprint(const list_t *l, FILE *out);
int list_print(const list_t *l);

void list_set_destroyer(list_t *l, void_dtr_t dtr);
void list_set_comparator(list_t *l, void_cmp_t cmp);
void list_set_copier(list_t *l, void_cpy_t cpy);

list_iterator_t *list_iterator_create(const list_t *l);
generic_t list_iterator_get_next(list_iterator_t *li);
bool list_iterator_has_next(const list_iterator_t *li);
void list_iterator_reset(list_iterator_t *li);
void list_iterator_destroy(list_iterator_t *li);

#endif
