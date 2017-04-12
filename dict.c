#include <stdlib.h>
#include <string.h>
#include "generic.h"
#include "mem.h"
#include "dict.h"
#include "htbl.h"
#include "bst.h"

static const dict_vtable_t _htbl_vtable = {
    .set_destroyer       = (f_dict_set_destroyer)htbl_set_destroyer,
    .set_comparator      = (f_dict_set_comparator)htbl_set_comparator,
    .take_data_ownership = (f_dict_take_data_ownership)htbl_take_data_ownership,
    .drop_data_ownership = (f_dict_drop_data_ownership)htbl_drop_data_ownership,
    .clear               = (f_dict_clear)htbl_clear,
    .put                 = (f_dict_put)htbl_put,
    .get                 = (f_dict_get)htbl_get,
    .pop                 = (f_dict_pop)htbl_pop,
    .has_key             = (f_dict_has_key)htbl_has_key,
    .get_size            = (f_dict_get_size)htbl_get_size,
    .is_empty            = (f_dict_is_empty)htbl_is_empty,
    .serialize           = (f_dict_serialize)htbl_serialize,
    .as_str              = (f_dict_as_str)htbl_as_str,
    .print               = (f_dict_print)htbl_print,
    .fprint              = (f_dict_fprint)htbl_fprint,
};

static const dict_vtable_t _bst_vtable = {
    .set_destroyer       = (f_dict_set_destroyer)bst_set_destroyer,
    .set_comparator      = (f_dict_set_comparator)bst_set_comparator,
    .take_data_ownership = (f_dict_take_data_ownership)bst_take_data_ownership,
    .drop_data_ownership = (f_dict_drop_data_ownership)bst_drop_data_ownership,
    .clear               = (f_dict_clear)bst_clear,
    .put                 = (f_dict_put)bst_put,
    .get                 = (f_dict_get)bst_get,
    .pop                 = (f_dict_pop)bst_pop,
    .has_key             = (f_dict_has_key)bst_has_key,
    .get_size            = (f_dict_get_size)bst_get_size,
    .is_empty            = (f_dict_is_empty)bst_is_empty,
    .serialize           = (f_dict_serialize)bst_serialize,
    .as_str              = (f_dict_as_str)bst_as_str,
    .print               = (f_dict_print)bst_print,
    .fprint              = (f_dict_fprint)bst_fprint,
};

static const dict_iterator_vtable_t _htbl_iterator_vtable = {
    .next                = (f_dict_iterator_get_next)htbl_iterator_get_next,
    .has_next            = (f_dict_iterator_has_next)htbl_iterator_has_next,
    .reset               = (f_dict_iterator_reset)htbl_iterator_reset,
};

static const dict_iterator_vtable_t _bst_iterator_vtable = {
    .next                = (f_dict_iterator_get_next)bst_iterator_get_next,
    .has_next            = (f_dict_iterator_has_next)bst_iterator_has_next,
    .reset               = (f_dict_iterator_reset)bst_iterator_reset,
};

static dict_backend_t _default_backend = DICT_BACKEND_HTBL;

void libgnrc_dict_set_default_backend(dict_backend_t backend)
{
    ASSERT_INPUT(backend > DICT_BACKEND_DEFAULT);
    ASSERT_INPUT(backend < DICT_BACKENDS_COUNT);

    _default_backend = backend;
}

dict_backend_t libgnrc_dict_get_default_backend(void)
{
    return _default_backend;
}

dict_t *dict_create(dict_backend_t backend)
{
    ASSERT_INPUT(backend < DICT_BACKENDS_COUNT);

    dict_t *d = umalloc(sizeof(*d));
    d->backend = (backend == DICT_BACKEND_DEFAULT) ? _default_backend : backend;
    switch (d->backend)
    {
        case DICT_BACKEND_HTBL:
            d->vobj = htbl_create();
            d->vtable = &_htbl_vtable;
            break;
        case DICT_BACKEND_BST_PLAIN:
            d->vobj = bst_create_ext(BST_NO_BALANCING);
            d->vtable = &_bst_vtable;
            break;
        case DICT_BACKEND_BST_RB:
            d->vobj = bst_create_ext(BST_RB_BALANCING);
            d->vtable = &_bst_vtable;
            break;
        default:
            ABORT("internal error");
    }
    return d;
}

void dict_destroy(dict_t *d)
{
    ASSERT_INPUT(d);

    switch (d->backend)
    {
        case DICT_BACKEND_HTBL:
            htbl_destroy(d->vobj);
            break;
        case DICT_BACKEND_BST_PLAIN:
        case DICT_BACKEND_BST_RB:
            bst_destroy(d->vobj);
            break;
        default:
            ABORT("internal error");
    }
    ufree(d);
}

dict_iterator_t *dict_iterator_create(const dict_t *d)
{
    ASSERT_INPUT(d);

    dict_iterator_t *di = umalloc(sizeof(*di));
    di->dict = d;
    switch (d->backend)
    {
        case DICT_BACKEND_HTBL:
            di->vobj = htbl_iterator_create(d->vobj);
            di->vtable = &_htbl_iterator_vtable;
            break;
        case DICT_BACKEND_BST_PLAIN:
        case DICT_BACKEND_BST_RB:
            di->vobj = bst_iterator_create(d->vobj);
            di->vtable = &_bst_iterator_vtable;
            break;
        default:
            ABORT("internal error");
    }

    return di;
}

void dict_iterator_destroy(dict_iterator_t *di)
{
    if (di)
    {
        switch (di->dict->backend)
        {
            case DICT_BACKEND_HTBL:
                htbl_iterator_destroy(di->vobj);
                break;
            case DICT_BACKEND_BST_PLAIN:
            case DICT_BACKEND_BST_RB:
                bst_iterator_destroy(di->vobj);
                break;
            default:
                ABORT("internal error");
        }
        ufree(di);
    }
}

dict_t *dict_deep_copy(const dict_t *d)
{
    (void)d;
    ABORT("not implemented");
}
