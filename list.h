#ifndef ULIST_H__
#define ULIST_H__

#include "generic.h"

typedef struct ulist_opaq ulist_t;
typedef struct ulist_iterator_opaq ulist_iterator_t;

ulist_t *ulist_create(void);
void ulist_clear(ulist_t *l);
void ulist_destroy(ulist_t *l);
void ulist_append(ulist_t *l, ugeneric_t e);
void ulist_prepend(ulist_t *l, ugeneric_t e);
ugeneric_t ulist_pop_back(ulist_t *l);
ugeneric_t ulist_pop_front(ulist_t *l);
bool ulist_is_empty(const ulist_t *l);
size_t ulist_get_size(const ulist_t *l);
ugeneric_t ulist_get_at(const ulist_t *l, size_t i);
void ulist_set_at(ulist_t *l, size_t i, ugeneric_t e);
void ulist_insert_at(ulist_t *l, size_t i, ugeneric_t e);
void ulist_remove_at(ulist_t *l, size_t i);
bool ulist_contains(const ulist_t *l, ugeneric_t e);
ugeneric_t *ulist_find(ulist_t *l, ugeneric_t e);
void ulist_reverse(ulist_t *l);
ulist_t *ulist_copy(const ulist_t *l);
ulist_t *ulist_deep_copy(const ulist_t *l);
int ulist_compare(const ulist_t *l1, const ulist_t *l2, void_cmp_t cmp);

char *ulist_as_str(const ulist_t *l);
void ulist_serialize(const ulist_t *l, ubuffer_t *buf);
int ulist_fprint(const ulist_t *l, FILE *out);
static inline int ulist_print(const ulist_t *l) {return ulist_fprint(l, stdout);}

static void ulist_take_data_ownership(ulist_t *l);
static void ulist_drop_data_ownership(ulist_t *l);
static bool ulist_is_data_owner(ulist_t *l);

ulist_iterator_t *ulist_iterator_create(const ulist_t *l);
ugeneric_t ulist_iterator_get_next(ulist_iterator_t *li);
ugeneric_t *ulist_iterator_get_next_ref(ulist_iterator_t *li);
bool ulist_iterator_has_next(const ulist_iterator_t *li);
void ulist_iterator_reset(ulist_iterator_t *li);
void ulist_iterator_destroy(ulist_iterator_t *li);

ugeneric_base_t *ulist_get_base(ulist_t *l);
DEFINE_BASE_FUNCS(ulist, l)

#endif
