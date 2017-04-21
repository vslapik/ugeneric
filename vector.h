#ifndef VECTOR_H__
#define VECTOR_H__

#include "generic.h"

#define VECTOR_INITIAL_CAPACITY 16

typedef struct vector_opaq vector_t;

vector_t *vector_create(void);
vector_t *vector_create_with_size(size_t size, generic_t value);
vector_t *vector_create_from_array(void *array, size_t array_len,
                                   size_t array_element_size,
                                   generic_type_e vector_element_type);
void vector_destroy(vector_t *v);
void vector_clear(vector_t *v);

vector_t *vector_copy(const vector_t *v);
vector_t *vector_deep_copy(const vector_t *v);
int vector_compare(const vector_t *v1, const vector_t *v2, void_cmp_t cmp);

void vector_append(vector_t *v, generic_t e);
void vector_insert_at(vector_t *v, size_t i, generic_t e);
void vector_remove_at(vector_t *v, size_t i);
generic_t vector_pop_at(vector_t *v, size_t i);
generic_t vector_pop_back(vector_t *v);
generic_t vector_get_back(const vector_t *v);
generic_t vector_get_at(const vector_t *v, size_t i);
generic_t vector_get_at_random(const vector_t *v);
void vector_set_at(vector_t *v, size_t i, generic_t e);
generic_t *vector_get_cells(const vector_t *v);

bool vector_is_empty(const vector_t *v);
size_t vector_get_size(const vector_t *v);
void vector_resize(vector_t *v, size_t new_size, generic_t value);
void vector_shrink_to_size(vector_t *v);

void vector_reserve_capacity(vector_t *v, size_t new_capacity);
size_t vector_get_capacity(const vector_t *v);

void vector_swap(vector_t *v, size_t l, size_t r);
void vector_reverse(vector_t *v, size_t l, size_t r);
void vector_sort(vector_t *v);
size_t vector_bsearch(const vector_t *v, generic_t e);
bool vector_next_permutation(vector_t *v);

void vector_take_data_ownership(vector_t *v);
void vector_drop_data_ownership(vector_t *v);
void vector_set_destroyer(vector_t *v, void_dtr_t dtr);
void vector_set_comparator(vector_t *v, void_cmp_t cmp);
void vector_set_copier(vector_t *v, void_cpy_t cpy);
void_dtr_t vector_get_destroyer(const vector_t *v);
void_cmp_t vector_get_comparator(const vector_t *v);
void_cpy_t vector_get_copier(const vector_t *v);

char *vector_as_str(const vector_t *v);
void vector_serialize(const vector_t *v, buffer_t *buf);
int vector_print(const vector_t *v);
int vector_fprint(const vector_t *v, FILE *out);

#endif
