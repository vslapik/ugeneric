#ifndef UDEQUE_H__
#define UDEQUE_H__

#include "generic.h"

typedef struct deque_opaq deque_t;

deque_t *deque_create(size_t size, ugeneric_t value);
deque_t *deque_create_empty(void);
deque_t *deque_create_from_array(ugeneric_t *array, size_t array_len);
void deque_take_data_ownership(deque_t *d);
void deque_drop_data_ownership(deque_t *d);
void deque_destroy(deque_t *d);
void deque_clear(deque_t *d);
void deque_append(deque_t *d, ugeneric_t element);
ugeneric_t deque_peek_back(const deque_t *d);
ugeneric_t deque_peek_front(const deque_t *d);
ugeneric_t deque_pop_back(deque_t *d);
ugeneric_t deque_pop_front(deque_t *d);
ugeneric_t deque_get_at(const deque_t *d, size_t i);
ugeneric_t deque_get_at_random(const deque_t *d);
void deque_set_at(deque_t *d, size_t i, ugeneric_t e);
ugeneric_t *deque_get_cells(const deque_t *d);
size_t deque_get_size(const deque_t *d);
void deque_resize(deque_t *d, size_t new_size, ugeneric_t value);
void deque_reserve_capacity(deque_t *d, size_t new_capacity);
size_t deque_get_capacity(const deque_t *d);
void deque_swap(deque_t *d, size_t l, size_t r);
void deque_reverse(deque_t *d, size_t l, size_t r);
bool deque_is_empty(const deque_t *d);
void deque_insert_at(deque_t *d, size_t i, ugeneric_t e);
void deque_remove_at(deque_t *d, size_t i);
void deque_sort(deque_t *d);
size_t deque_bsearch(const deque_t *d, ugeneric_t e);
deque_t *deque_copy(const deque_t *d);
deque_t *deque_deep_copy(const deque_t *d);
int deque_compare(const deque_t *d1, const deque_t *d2, void_cmp_t cmp);
bool deque_next_permutation(deque_t *d);

void deque_serialize(const deque_t *d, ubuffer_t *buf);
char *deque_as_str(const deque_t *d);
void deque_print(const deque_t *d);
void deque_fprint(const deque_t *d, FILE *out);

#endif
