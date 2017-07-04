#include <stdlib.h>
#include <string.h>
#include "mem.h"
#include "list.h"

/* head->[e{0}]->[e{1}]-> ... >[e{size-2}]->[e{size-1}]->NULL  */

typedef struct ulist_item {
    ugeneric_t data;
    struct ulist_item *next;
} ulist_item_t;

struct ulist_opaq {
    uvoid_handlers_t void_handlers;
    size_t size;
    ulist_item_t *head;
    bool is_data_owner;
};

struct ulist_iterator_opaq {
    const ulist_t *list;
    ulist_item_t *next;
};

static inline ulist_item_t **_rewind_to(ulist_t *l, size_t i)
{
    ulist_item_t **t = &l->head;
    while (i--)
    {
        t = &(*t)->next;
    }

    return t;
}

static ulist_t *_lcpy(const ulist_t *l, bool deep)
{

    ulist_t *copy = ulist_create();
    memcpy(copy, l, sizeof(*l));
    ulist_item_t *from = l->head;
    ulist_item_t **to = &copy->head;

    copy->is_data_owner = deep;
    if (deep)
    {
        while (from)
        {
            *to = umalloc(sizeof(**to));
            (*to)->data = ugeneric_copy(from->data, l->void_handlers.cpy);
            (*to)->next = NULL;
            to = &(*to)->next;
            from = from->next;
        }
    }
    else
    {
        while (from)
        {
            *to = umalloc(sizeof(**to));
            (*to)->data = from->data;
            (*to)->next = NULL;
            to = &(*to)->next;
            from = from->next;
        }
    }

    return copy;
}

ulist_t *ulist_create(void)
{
    ulist_t *l = umalloc(sizeof(*l));

    l->size = 0;
    l->is_data_owner = true;
    l->head = NULL;
    memset(&l->void_handlers, 0, sizeof(l->void_handlers));

    return l;
}

void ulist_take_data_ownership(ulist_t *l)
{
    UASSERT_INPUT(l);
    l->is_data_owner = true;
}

void ulist_drop_data_ownership(ulist_t *l)
{
    UASSERT_INPUT(l);
    l->is_data_owner = false;
}

void ulist_append(ulist_t *l, ugeneric_t e)
{
    UASSERT_INPUT(l);
    ulist_item_t *li = umalloc(sizeof(*li));

    li->data = e;
    li->next = NULL;

    *_rewind_to(l, l->size) = li;
    l->size++;
}

void ulist_prepend(ulist_t *l, ugeneric_t e)
{
    UASSERT_INPUT(l);
    ulist_item_t *li = umalloc(sizeof(*li));

    li->next = l->head;
    li->data = e;
    l->head = li;
    l->size++;
}

ugeneric_t ulist_pop_back(ulist_t *l)
{
    UASSERT_INPUT(l);
    UASSERT_INPUT(l->size);

    ulist_item_t **t = _rewind_to(l, l->size - 1);
    ugeneric_t e = (*t)->data;
    ufree(*t);
    *t = NULL;
    l->size--;

    return e;
}

ugeneric_t ulist_pop_front(ulist_t *l)
{
    UASSERT_INPUT(l);
    UASSERT_INPUT(l->size);

    ulist_item_t *li = l->head;
    ugeneric_t e = li->data;
    l->head = li->next;
    ufree(li);
    l->size--;

    return e;
}

void ulist_destroy(ulist_t *l)
{
    if (l)
    {
        ulist_item_t *li = l->head;
        ugeneric_t g;

        if (l->is_data_owner)
        {
            while (li)
            {
                g = li->data;
                ugeneric_destroy(g, l->void_handlers.dtr);
                li = li->next;
            }
        }
        ulist_clear(l);
        ufree(l);
    }
}

void ulist_clear(ulist_t *l)
{
    UASSERT_INPUT(l);

    ulist_item_t *li = l->head;
    ulist_item_t *t;

    while (li)
    {
        t = li;
        li = li->next;
        ufree(t);
    }
    l->head = NULL;
}

bool ulist_is_empty(const ulist_t *l)
{
    UASSERT_INPUT(l);
    return l->size == 0;
}

size_t ulist_get_size(const ulist_t *l)
{
    UASSERT_INPUT(l);
    return l->size;
}

ugeneric_t ulist_get_at(const ulist_t *l, size_t i)
{
    UASSERT_INPUT(l);
    UASSERT_INPUT(i < l->size);

    ulist_item_t **t = _rewind_to((ulist_t*)l, i);

    return (*t)->data;
}

void ulist_set_at(ulist_t *l, size_t i, ugeneric_t e)
{
    UASSERT_INPUT(l);
    UASSERT_INPUT(i < l->size);

    ulist_item_t **t = _rewind_to(l, i);
    (*t)->data = e;
}

void ulist_insert_at(ulist_t *l, size_t i, ugeneric_t e)
{
    UASSERT_INPUT(l);
    UASSERT_INPUT(i < l->size);

    ulist_item_t *li = umalloc(sizeof(*li));
    li->data = e;

    ulist_item_t **t = _rewind_to(l, i);

    li->next = *t;
    *t = li;
    l->size++;
}

void ulist_remove_at(ulist_t *l, size_t i)
{
    UASSERT_INPUT(l);
    UASSERT_INPUT(i < l->size);

    ulist_item_t **t = _rewind_to(l, i);
    ulist_item_t *f = *t;

    *t = (*t)->next;
    ufree(f);

    l->size--;
}

bool ulist_contains(const ulist_t *l, ugeneric_t e)
{
    UASSERT_INPUT(l);

    ulist_item_t *li = l->head;
    ulist_item_t *t;
    bool ret = false;

    while (li)
    {
        t = li;
        li = li->next;
        if (ugeneric_compare(t->data, e, l->void_handlers.cmp) == 0)
        {
            ret = true;
            break;
        }
    }

    return ret;
}

void ulist_reverse(ulist_t *l)
{
    UASSERT_INPUT(l);
    UABORT("not implemented");
    ulist_item_t *li = l->head;

    while (li)
    {
    }
}

int ulist_compare(const ulist_t *l1, const ulist_t *l2, void_cmp_t cmp)
{
    UASSERT_INPUT(l1);
    UASSERT_INPUT(l2);

    ulist_item_t *l = l1->head;
    ulist_item_t *r = l2->head;

    while (l && r)
    {
        int diff = ugeneric_compare(l->data, r->data, cmp);
        if (diff == 0)
        {
            l = l->next;
            r = r->next;
        }
        else
        {
            return diff;
        }
    }
    return l1->size - l2->size;
}

ulist_t *ulist_copy(const ulist_t *l)
{
    UASSERT_INPUT(l);
    return _lcpy(l, false);
}

ulist_t *ulist_deep_copy(const ulist_t *l)
{
    UASSERT_INPUT(l);
    return _lcpy(l, true);
}

int ulist_print(const ulist_t *l)
{
    UASSERT_INPUT(l);
    return ulist_fprint(l, stdout);
}

int ulist_fprint(const ulist_t *l, FILE *out)
{
    UASSERT_INPUT(l);
    UASSERT_INPUT(out);

    char *str = ulist_as_str(l);
    int ret = fprintf(out, "%s\n", str);
    ufree(str);

    return ret;
}

void ulist_serialize(const ulist_t *l, ubuffer_t *buf)
{
    UASSERT_INPUT(l);
    UASSERT_INPUT(buf);

    size_t i = 0;
    ulist_item_t *li = l->head;
    ubuffer_append_byte(buf, '[');
    while (li)
    {
        ugeneric_serialize_v(li->data, buf, l->void_handlers.s8r);
        li = li->next;
        if (i++ < l->size - 1)
        {
            ubuffer_append_data(buf, ", ", 2);
        }
    }
    ubuffer_append_byte(buf, ']');
}

char *ulist_as_str(const ulist_t *l)
{
    UASSERT_INPUT(l);

    ubuffer_t buf = {0};
    ulist_serialize(l, &buf);
    ubuffer_null_terminate(&buf);

    return buf.data;
}

ulist_iterator_t *ulist_iterator_create(const ulist_t *l)
{
    UASSERT_INPUT(l);
    ulist_iterator_t *li = umalloc(sizeof(*li));

    li->list = l;
    li->next = l->head;

    return li;
}

ugeneric_t ulist_iterator_get_next(ulist_iterator_t *li)
{
    UASSERT_INPUT(li);
    UASSERT_MSG(li->list->size, "container is empty");
    UASSERT_MSG(li->next, "iteration is done");

    ugeneric_t g = li->next->data;
    li->next = li->next->next;

    return g;
}

bool ulist_iterator_has_next(const ulist_iterator_t *li)
{
    return li->next;
}

void ulist_iterator_destroy(ulist_iterator_t *li)
{
    if (li)
    {
        ufree(li);
    }
}

void ulist_iterator_reset(ulist_iterator_t *li)
{
    li->next = li->list->head;
}

uvoid_handlers_t *ulist_get_void_handlers(ulist_t *l)
{
    UASSERT_INPUT(l);
    return &l->void_handlers;
}
