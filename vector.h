#ifndef UVECTOR_H__
#define UVECTOR_H__

#include "generic.h"

#define VECTOR_INITIAL_CAPACITY 16

typedef struct uvector_opaq uvector_t;

uvector_t *uvector_create(void);
uvector_t *uvector_create_with_size(size_t size, ugeneric_t value);
uvector_t *uvector_create_from_array(void *array, size_t array_len,
                                   size_t array_element_size,
                                   ugeneric_type_e uvector_element_type);
void uvector_destroy(uvector_t *v);
void uvector_clear(uvector_t *v);

uvector_t *uvector_copy(const uvector_t *v);
uvector_t *uvector_deep_copy(const uvector_t *v);
int uvector_compare(const uvector_t *v1, const uvector_t *v2, void_cmp_t cmp);

void uvector_append(uvector_t *v, ugeneric_t e);
void uvector_insert_at(uvector_t *v, size_t i, ugeneric_t e);
void uvector_remove_at(uvector_t *v, size_t i);
ugeneric_t uvector_pop_at(uvector_t *v, size_t i);
ugeneric_t uvector_pop_back(uvector_t *v);
ugeneric_t uvector_get_back(const uvector_t *v);
ugeneric_t uvector_get_at(const uvector_t *v, size_t i);
ugeneric_t uvector_get_at_random(const uvector_t *v);
void uvector_set_at(uvector_t *v, size_t i, ugeneric_t e);
ugeneric_t *uvector_get_cells(const uvector_t *v);

bool uvector_is_empty(const uvector_t *v);
size_t uvector_get_size(const uvector_t *v);
void uvector_resize(uvector_t *v, size_t new_size, ugeneric_t value);
void uvector_shrink_to_size(uvector_t *v);

void uvector_reserve_capacity(uvector_t *v, size_t new_capacity);
size_t uvector_get_capacity(const uvector_t *v);

void uvector_swap(uvector_t *v, size_t l, size_t r);
void uvector_reverse(uvector_t *v, size_t l, size_t r);
void uvector_sort(uvector_t *v);
size_t uvector_bsearch(const uvector_t *v, ugeneric_t e);
bool uvector_next_permutation(uvector_t *v);

void uvector_take_data_ownership(uvector_t *v);
void uvector_drop_data_ownership(uvector_t *v);
void uvector_set_destroyer(uvector_t *v, void_dtr_t dtr);
void uvector_set_comparator(uvector_t *v, void_cmp_t cmp);
void uvector_set_copier(uvector_t *v, void_cpy_t cpy);
void uvector_set_void_serializer(uvector_t *v, void_s8r_t serializer);
void_dtr_t uvector_get_destroyer(const uvector_t *v);
void_cmp_t uvector_get_comparator(const uvector_t *v);
void_cpy_t uvector_get_copier(const uvector_t *v);
void_s8r_t uvector_get_void_serializer(const uvector_t *v);

char *uvector_as_str(const uvector_t *v);
void uvector_serialize(const uvector_t *v, ubuffer_t *buf);
int uvector_print(const uvector_t *v);
int uvector_fprint(const uvector_t *v, FILE *out);

#endif
