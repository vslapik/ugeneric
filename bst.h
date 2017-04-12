#ifndef BST_H__
#define BST_H__

#include "generic.h"

typedef struct bst_opaq bst_t;
typedef struct bst_iterator_opaq bst_iterator_t;

typedef enum {
    BST_PREORDER,
    BST_POSTORDER,
    BST_INORDER,
} bst_traverse_mode_t;

typedef enum {
    BST_DEFAULT_BALANCING,
    BST_NO_BALANCING,
    BST_RB_BALANCING,
    BST_SPLAY_BALANCING,
    BST_BALANCING_MODES_COUNT, // keep it last
} bst_balancing_mode_t;

void bst_set_default_balancing_mode(bst_balancing_mode_t mode);

bst_t *bst_create(void);
bst_t *bst_create_ext(bst_balancing_mode_t mode);
void bst_set_destroyer(bst_t *b, void_dtr_t dtr);
void bst_set_comparator(bst_t *b, void_cmp_t cmp);
void bst_destroy(bst_t *b);
void bst_drop_data_ownership(bst_t *b);
void bst_take_data_ownership(bst_t *b);

void bst_put(bst_t *b, generic_t k, generic_t v);
generic_t bst_pop(bst_t *b, generic_t k, generic_t vdef);
generic_t bst_get(bst_t *b, generic_t k, generic_t vdef);
bool bst_has_key(const bst_t *b, generic_t k);
generic_t bst_get_min(bst_t *b);
generic_t bst_get_max(bst_t *b);
size_t bst_get_size(bst_t *b);
bool bst_is_empty(bst_t *b);
void bst_clear(bst_t *b);
generic_t bst_get_inorder_predecessor(bst_t *b, generic_t k, generic_t vdef);
generic_t bst_get_inorder_successor(bst_t *b, generic_t k, generic_t vdef);
bool bst_is_balanced(const bst_t *b);

void bst_traverse(const bst_t *b, bst_traverse_mode_t mode,
                  generic_kv_iter_t iter, void *data);
void bst_dump_to_dot(const bst_t *b, const char *name, bool dump_values,
                     FILE *out);

char *bst_as_str(const bst_t *b);
void bst_serialize(const bst_t *b, buffer_t *buf);
int bst_fprint(const bst_t *b, FILE *out);
int bst_print(const bst_t *b);

void rotate_left(bst_t *b);
void rotate_right(bst_t *b);

bst_iterator_t *bst_iterator_create(const bst_t *b);
generic_kv_t bst_iterator_get_next(bst_iterator_t *bi);
bool bst_iterator_has_next(const bst_iterator_t *bi);
void bst_iterator_reset(bst_iterator_t *bi);
void bst_iterator_destroy(bst_iterator_t *bi);

#endif
