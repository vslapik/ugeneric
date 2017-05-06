#ifndef UBST_H__
#define UBST_H__

#include "generic.h"

typedef struct ubst_opaq ubst_t;
typedef struct ubst_iterator_opaq ubst_iterator_t;


typedef enum {
    UBST_PREORDER,
    UBST_POSTORDER,
    UBST_INORDER,
} ubst_traverse_mode_t;

typedef enum {
    UBST_DEFAULT_BALANCING,
    UBST_NO_BALANCING,
    UBST_RB_BALANCING,
    UBST_SPLAY_BALANCING,
    UBST_BALANCING_MODES_COUNT, // keep it last
} ubst_balancing_mode_t;

void ubst_set_default_balancing_mode(ubst_balancing_mode_t mode);

ubst_t *ubst_create(void);
ubst_t *ubst_create_ext(ubst_balancing_mode_t mode);
void ubst_set_destroyer(ubst_t *b, void_dtr_t dtr);
void ubst_set_comparator(ubst_t *b, void_cmp_t cmp);
void ubst_destroy(ubst_t *b);
void ubst_drop_data_ownership(ubst_t *b);
void ubst_take_data_ownership(ubst_t *b);

void ubst_put(ubst_t *b, ugeneric_t k, ugeneric_t v);
ugeneric_t ubst_pop(ubst_t *b, ugeneric_t k, ugeneric_t vdef);
ugeneric_t ubst_get(ubst_t *b, ugeneric_t k, ugeneric_t vdef);
bool ubst_has_key(const ubst_t *b, ugeneric_t k);
ugeneric_t ubst_get_min(ubst_t *b);
ugeneric_t ubst_get_max(ubst_t *b);
size_t ubst_get_size(ubst_t *b);
bool ubst_is_empty(ubst_t *b);
void ubst_clear(ubst_t *b);
ugeneric_t ubst_get_inorder_predecessor(ubst_t *b,
                                        ugeneric_t k, ugeneric_t vdef);
ugeneric_t ubst_get_inorder_successor(ubst_t *b,
                                      ugeneric_t k, ugeneric_t vdef);
bool ubst_is_balanced(const ubst_t *b);

void ubst_traverse(const ubst_t *b, ubst_traverse_mode_t mode,
                   ugeneric_kv_iter_t iter, void *data);
void ubst_dump_to_dot(const ubst_t *b, const char *name, bool dump_values,
                      FILE *out);

char *ubst_as_str(const ubst_t *b);
void ubst_serialize(const ubst_t *b, ubuffer_t *buf);
int ubst_fprint(const ubst_t *b, FILE *out);
int ubst_print(const ubst_t *b);

ubst_iterator_t *ubst_iterator_create(const ubst_t *b);
ugeneric_kv_t ubst_iterator_get_next(ubst_iterator_t *bi);
bool ubst_iterator_has_next(const ubst_iterator_t *bi);
void ubst_iterator_reset(ubst_iterator_t *bi);
void ubst_iterator_destroy(ubst_iterator_t *bi);

#endif
