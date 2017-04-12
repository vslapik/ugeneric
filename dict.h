#ifndef DICT_H__
#define DICT_H__

#include "generic.h"

typedef enum {
    DICT_BACKEND_DEFAULT,
    DICT_BACKEND_BST_PLAIN,
    DICT_BACKEND_BST_RB,
    //DICT_BACKEND_BST_SPLAY,
    DICT_BACKEND_HTBL,
    DICT_BACKENDS_COUNT, // keep it last
} dict_backend_t;

void libgnrc_dict_set_default_backend(dict_backend_t backend);
dict_backend_t libgnrc_dict_get_default_backend(void);

typedef void         (*f_dict_set_destroyer)(void *d, void_dtr_t dtr);
typedef void         (*f_dict_set_comparator)(void *d, void_cmp_t cmp);
typedef void         (*f_dict_take_data_ownership)(void *d);
typedef void         (*f_dict_drop_data_ownership)(void *d);
typedef void         (*f_dict_clear)(void *d);
typedef void         (*f_dict_put)(void *d, generic_t k, generic_t v);
typedef generic_t    (*f_dict_get)(const void *d, generic_t k, generic_t vdef);
typedef generic_t    (*f_dict_pop)(void *d, generic_t k, generic_t vdef);
typedef bool         (*f_dict_has_key)(const void *d, generic_t k);
typedef size_t       (*f_dict_get_size)(const void *d);
typedef bool         (*f_dict_is_empty)(const void *d);
typedef void         (*f_dict_serialize)(const void *d, buffer_t *buf);
typedef char        *(*f_dict_as_str)(const void *d);
typedef int          (*f_dict_print)(const void *d);
typedef int          (*f_dict_fprint)(const void *d, FILE *out);

typedef generic_kv_t (*f_dict_iterator_get_next)(void *di);
typedef bool         (*f_dict_iterator_has_next)(const void *di);
typedef void         (*f_dict_iterator_reset)(void *di);


typedef struct {
    f_dict_set_destroyer       set_destroyer;
    f_dict_set_comparator      set_comparator;
    f_dict_take_data_ownership take_data_ownership;
    f_dict_drop_data_ownership drop_data_ownership;
    f_dict_clear               clear;
    f_dict_put                 put;
    f_dict_get                 get;
    f_dict_pop                 pop;
    f_dict_has_key             has_key;
    f_dict_get_size            get_size;
    f_dict_is_empty            is_empty;
    f_dict_serialize           serialize;
    f_dict_as_str              as_str;
    f_dict_print               print;
    f_dict_fprint              fprint;
} dict_vtable_t;

typedef struct {
    f_dict_iterator_get_next   next;
    f_dict_iterator_has_next   has_next;
    f_dict_iterator_reset      reset;
} dict_iterator_vtable_t;

typedef struct {
    dict_backend_t backend;
    void *vobj;
    const dict_vtable_t *vtable;
} dict_t;

typedef struct {
    dict_backend_t backend;
    const dict_t *dict;
    void *vobj;
    const dict_iterator_vtable_t *vtable;
} dict_iterator_t;

dict_t *dict_create(dict_backend_t backend);
static inline void dict_set_destroyer(dict_t *d, void_dtr_t dtr) {d->vtable->set_destroyer(d->vobj, dtr);}
static inline void dict_set_comparator(dict_t *d, void_cmp_t cmp) {d->vtable->set_comparator(d->vobj, cmp);}
static inline void dict_take_data_ownership(dict_t *d) {d->vtable->take_data_ownership(d->vobj);}
static inline void dict_drop_data_ownership(dict_t *d) {d->vtable->drop_data_ownership(d->vobj);}
static inline void dict_clear(dict_t *d) {d->vtable->clear(d->vobj);}
static inline void dict_put(dict_t *d, generic_t k, generic_t v) {d->vtable->put(d->vobj, k, v);}
static inline generic_t dict_get(const dict_t *d, generic_t k, generic_t vdef) {return d->vtable->get(d->vobj, k, vdef);}
static inline generic_t dict_pop(dict_t *d, generic_t k, generic_t vdef) {return d->vtable->pop(d->vobj, k, vdef);}
static inline bool dict_has_key(const dict_t *d, generic_t k) {return d->vtable->has_key(d->vobj, k);}
static inline size_t dict_get_size(const dict_t *d) {return d->vtable->get_size(d->vobj);}
static inline bool dict_is_empty(const dict_t *d) {return d->vtable->is_empty(d->vobj);}
static inline void dict_serialize(const dict_t *d, buffer_t *buf) {d->vtable->serialize(d->vobj, buf);}
static inline char *dict_as_str(const dict_t *d) {return d->vtable->as_str(d->vobj);}
static inline int dict_print(const dict_t *d) {return d->vtable->print(d->vobj);}
static inline int dict_fprint(const dict_t *d, FILE *out) {return d->vtable->fprint(d->vobj, out);}
void dict_destroy(dict_t *d);

dict_t *dict_deep_copy(const dict_t *v);

dict_iterator_t *dict_iterator_create(const dict_t *d);
static inline generic_kv_t dict_iterator_get_next(dict_iterator_t *di) {return di->vtable->next(di->vobj);}
static inline bool dict_iterator_has_next(const dict_iterator_t *di) {return di->vtable->has_next(di->vobj);}
static inline void dict_iterator_reset(dict_iterator_t *di) {di->vtable->reset(di->vobj);}
void dict_iterator_destroy(dict_iterator_t *di);

#endif
