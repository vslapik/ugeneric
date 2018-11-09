#ifndef USET_H__
#define USET_H__

#include "dict.h"

typedef struct {
    udict_t *data;
} uset_t;

uset_t *uset_create(void);

void uset_update_from_vector(uset_t *s, uvector_t *v, bool deep);
//void uset_update_from_vector(uset_t *s, const uvector_t *v, bool deep);
//uset_t *uset_update_from_list(const vector_t *v, bool deep);

void uset_destroy(uset_t *s);

static void uset_take_data_ownership(uset_t *s);
static void uset_drop_data_ownership(uset_t *s);
static bool uset_is_data_owner(uset_t *s);

static inline void uset_clear(uset_t *s) {udict_clear(s->data);}
static inline ugeneric_t uset_get(const uset_t *s, ugeneric_t k, ugeneric_t vdef) {return udict_get(s->data, k, vdef);}
static inline ugeneric_t uset_pop(uset_t *s, ugeneric_t k, ugeneric_t vdef) {return udict_pop(s->data, k, vdef);}
static inline size_t uset_get_size(const uset_t *s) {return udict_get_size(s->data);}
static inline bool uset_is_empty(const uset_t *s) {return udict_is_empty(s->data);}

void uset_serialize(const uset_t *s, ubuffer_t *buf);
char *uset_as_str(const uset_t *s);
int uset_fprint(const uset_t *s, FILE *out);
static inline int uset_print(const uset_t *s) {return uset_fprint(s, stdout);}

uvector_t *uset_get_elements(const uset_t *s, bool deep);

void uset_put(uset_t *s, ugeneric_t e);
size_t uset_has_element(const uset_t *s, ugeneric_t e);

bool uset_is_subset(const uset_t *s, const uset_t *t);
bool uset_is_superset(const uset_t *s, const uset_t *t);
uset_t *uset_make_union(const uset_t *s1, const uset_t *s2);
uset_t *uset_get_intersect(const uset_t *s1, const uset_t *s2);
uset_t *uset_get_diff(const uset_t *s1, const uset_t *s2);
uset_t *uset_get_symmetric_diff(const uset_t *s1, const uset_t *s2);
uset_t *uset_copy(const uset_t *s);

static inline ugeneric_base_t *uset_get_base(uset_t *s) {return udict_get_base(s->data);}
DEFINE_BASE_FUNCS(uset, s)

#endif
