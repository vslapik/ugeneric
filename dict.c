#include <stdlib.h>
#include <string.h>
#include "generic.h"
#include "mem.h"
#include "dict.h"
#include "htbl.h"
#include "bst.h"

static const udict_vtable_t _uhtbl_vtable = {
    .set_destroyer       = (f_udict_set_destroyer)uhtbl_set_destroyer,
    .set_comparator      = (f_udict_set_comparator)uhtbl_set_comparator,
    .take_data_ownership = (f_udict_take_data_ownership)uhtbl_take_data_ownership,
    .drop_data_ownership = (f_udict_drop_data_ownership)uhtbl_drop_data_ownership,
    .clear               = (f_udict_clear)uhtbl_clear,
    .put                 = (f_udict_put)uhtbl_put,
    .get                 = (f_udict_get)uhtbl_get,
    .pop                 = (f_udict_pop)uhtbl_pop,
    .has_key             = (f_udict_has_key)uhtbl_has_key,
    .get_size            = (f_udict_get_size)uhtbl_get_size,
    .is_empty            = (f_udict_is_empty)uhtbl_is_empty,
    .serialize           = (f_udict_serialize)uhtbl_serialize,
    .as_str              = (f_udict_as_str)uhtbl_as_str,
    .print               = (f_udict_print)uhtbl_print,
    .fprint              = (f_udict_fprint)uhtbl_fprint,
};

static const udict_vtable_t _ubst_vtable = {
    .set_destroyer       = (f_udict_set_destroyer)ubst_set_destroyer,
    .set_comparator      = (f_udict_set_comparator)ubst_set_comparator,
    .take_data_ownership = (f_udict_take_data_ownership)ubst_take_data_ownership,
    .drop_data_ownership = (f_udict_drop_data_ownership)ubst_drop_data_ownership,
    .clear               = (f_udict_clear)ubst_clear,
    .put                 = (f_udict_put)ubst_put,
    .get                 = (f_udict_get)ubst_get,
    .pop                 = (f_udict_pop)ubst_pop,
    .has_key             = (f_udict_has_key)ubst_has_key,
    .get_size            = (f_udict_get_size)ubst_get_size,
    .is_empty            = (f_udict_is_empty)ubst_is_empty,
    .serialize           = (f_udict_serialize)ubst_serialize,
    .as_str              = (f_udict_as_str)ubst_as_str,
    .print               = (f_udict_print)ubst_print,
    .fprint              = (f_udict_fprint)ubst_fprint,
};

static const udict_iterator_vtable_t _uhtbl_iterator_vtable = {
    .next                = (f_udict_iterator_get_next)uhtbl_iterator_get_next,
    .has_next            = (f_udict_iterator_has_next)uhtbl_iterator_has_next,
    .reset               = (f_udict_iterator_reset)uhtbl_iterator_reset,
};

static const udict_iterator_vtable_t _ubst_iterator_vtable = {
    .next                = (f_udict_iterator_get_next)ubst_iterator_get_next,
    .has_next            = (f_udict_iterator_has_next)ubst_iterator_has_next,
    .reset               = (f_udict_iterator_reset)ubst_iterator_reset,
};

static udict_backend_t _default_backend = UDICT_BACKEND_HTBL;

void libugeneric_udict_set_default_backend(udict_backend_t backend)
{
    UASSERT_INPUT(backend > UDICT_BACKEND_DEFAULT);
    UASSERT_INPUT(backend < UDICT_BACKENDS_COUNT);
    _default_backend = backend;
}

udict_backend_t libugeneric_udict_get_default_backend(void)
{
    return _default_backend;
}

udict_t *udict_create(void)
{
    return udict_create_with_backend(UDICT_BACKEND_DEFAULT);
}

udict_t *udict_create_with_backend(udict_backend_t backend)
{
    UASSERT_INPUT(backend >= UDICT_BACKEND_DEFAULT);
    UASSERT_INPUT(backend < UDICT_BACKENDS_COUNT);

    udict_t *d = umalloc(sizeof(*d));
    d->backend = (backend == UDICT_BACKEND_DEFAULT) ? _default_backend : backend;
    switch (d->backend)
    {
        case UDICT_BACKEND_HTBL:
            d->vobj = uhtbl_create();
            d->vtable = &_uhtbl_vtable;
            break;
        case UDICT_BACKEND_UBST_PLAIN:
            d->vobj = ubst_create_ext(UBST_NO_BALANCING);
            d->vtable = &_ubst_vtable;
            break;
        case UDICT_BACKEND_UBST_RB:
            d->vobj = ubst_create_ext(UBST_RB_BALANCING);
            d->vtable = &_ubst_vtable;
            break;
        default:
            UABORT("internal error");
    }
    return d;
}

void udict_destroy(udict_t *d)
{
    UASSERT_INPUT(d);

    switch (d->backend)
    {
        case UDICT_BACKEND_HTBL:
            uhtbl_destroy(d->vobj);
            break;
        case UDICT_BACKEND_UBST_PLAIN:
        case UDICT_BACKEND_UBST_RB:
            ubst_destroy(d->vobj);
            break;
        default:
            UABORT("internal error");
    }
    ufree(d);
}

udict_iterator_t *udict_iterator_create(const udict_t *d)
{
    UASSERT_INPUT(d);

    udict_iterator_t *di = umalloc(sizeof(*di));
    di->dict = d;
    switch (d->backend)
    {
        case UDICT_BACKEND_HTBL:
            di->vobj = uhtbl_iterator_create(d->vobj);
            di->vtable = &_uhtbl_iterator_vtable;
            break;
        case UDICT_BACKEND_UBST_PLAIN:
        case UDICT_BACKEND_UBST_RB:
            di->vobj = ubst_iterator_create(d->vobj);
            di->vtable = &_ubst_iterator_vtable;
            break;
        default:
            UABORT("internal error");
    }

    return di;
}

void udict_iterator_destroy(udict_iterator_t *di)
{
    if (di)
    {
        switch (di->dict->backend)
        {
            case UDICT_BACKEND_HTBL:
                uhtbl_iterator_destroy(di->vobj);
                break;
            case UDICT_BACKEND_UBST_PLAIN:
            case UDICT_BACKEND_UBST_RB:
                ubst_iterator_destroy(di->vobj);
                break;
            default:
                UABORT("internal error");
        }
        ufree(di);
    }
}

udict_t *udict_deep_copy(const udict_t *d)
{
    (void)d;
    UABORT("not implemented");
}
