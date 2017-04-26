#include <stdlib.h>
#include <string.h>
#include "mem.h"
#include "list.h"

/* head->[e{0}]->[e{1}]-> ... >[e{size-2}]->[e{size-1}]->NULL  */

typedef struct list_item {
    generic_t data;
    struct list_item *next;
} list_item_t;

struct list_opaq {
    size_t size;
    list_item_t *head;
    bool is_data_owner;
    void_cpy_t cpy;
    void_cmp_t cmp;
    void_dtr_t dtr;
    void_s8r_t void_serializer;
};

struct list_iterator_opaq {
    const list_t *list;
    list_item_t *next;
};

static inline list_item_t **_rewind_to(list_t *l, size_t i)
{
    list_item_t **t = &l->head;
    while (i--)
    {
        t = &(*t)->next;
    }

    return t;
}

static list_t *_lcpy(const list_t *l, bool deep)
{

    list_t *copy = list_create();
    memcpy(copy, l, sizeof(*l));
    list_item_t *from = l->head;
    list_item_t **to = &copy->head;

    if (deep)
    {
        copy->is_data_owner = true;
        while (from)
        {
            *to = umalloc(sizeof(**to));
            (*to)->data = generic_copy(from->data, l->cpy);
            (*to)->next = NULL;
            to = &(*to)->next;
            from = from->next;
        }
    }
    else
    {
        copy->is_data_owner = false;
        copy->cpy = 0;
        copy->cmp = 0;
        copy->dtr = 0;
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

list_t *list_create(void)
{
    list_t *l = umalloc(sizeof(*l));

    l->size = 0;
    l->is_data_owner = false;
    l->head = NULL;
    l->dtr = NULL;

    return l;
}

void list_take_data_ownership(list_t *l)
{
    ASSERT_INPUT(l);
    l->is_data_owner = true;
}

void list_drop_data_ownership(list_t *l)
{
    ASSERT_INPUT(l);
    l->is_data_owner = false;
}

void list_append(list_t *l, generic_t e)
{
    ASSERT_INPUT(l);
    list_item_t *li = umalloc(sizeof(*li));

    li->data = e;
    li->next = NULL;

    *_rewind_to(l, l->size) = li;
    l->size++;
}

void list_prepend(list_t *l, generic_t e)
{
    ASSERT_INPUT(l);
    list_item_t *li = umalloc(sizeof(*li));

    li->next = l->head;
    li->data = e;
    l->head = li;
    l->size++;
}

generic_t list_pop_back(list_t *l)
{
    ASSERT_INPUT(l);
    ASSERT_INPUT(l->size);

    list_item_t **t = _rewind_to(l, l->size - 1);
    generic_t e = (*t)->data;
    ufree(*t);
    *t = NULL;
    l->size--;

    return e;
}

generic_t list_pop_front(list_t *l)
{
    ASSERT_INPUT(l);
    ASSERT_INPUT(l->size);

    list_item_t *li = l->head;
    generic_t e = li->data;
    l->head = li->next;
    ufree(li);
    l->size--;

    return e;
}

void list_destroy(list_t *l)
{
    if (l)
    {
        list_item_t *li = l->head;
        generic_t g;

        if (l->is_data_owner)
        {
            while (li)
            {
                g = li->data;
                generic_destroy(g, l->dtr);
                li = li->next;
            }
        }
        list_clear(l);
        ufree(l);
    }
}

void list_clear(list_t *l)
{
    ASSERT_INPUT(l);

    list_item_t *li = l->head;
    list_item_t *t;

    while (li)
    {
        t = li;
        li = li->next;
        ufree(t);
    }
    l->head = NULL;
}

bool list_is_empty(const list_t *l)
{
    ASSERT_INPUT(l);
    return l->size == 0;
}

size_t list_get_size(const list_t *l)
{
    ASSERT_INPUT(l);
    return l->size;
}

generic_t list_get_at(const list_t *l, size_t i)
{
    ASSERT_INPUT(l);
    ASSERT_INPUT(i < l->size);

    list_item_t **t = _rewind_to((list_t*)l, i);

    return (*t)->data;
}

void list_set_at(list_t *l, size_t i, generic_t e)
{
    ASSERT_INPUT(l);
    ASSERT_INPUT(i < l->size);

    list_item_t **t = _rewind_to(l, i);
    (*t)->data = e;
}

void list_insert_at(list_t *l, size_t i, generic_t e)
{
    ASSERT_INPUT(l);
    ASSERT_INPUT(i < l->size);

    list_item_t *li = umalloc(sizeof(*li));
    li->data = e;

    list_item_t **t = _rewind_to(l, i);

    li->next = *t;
    *t = li;
    l->size++;
}

void list_remove_at(list_t *l, size_t i)
{
    ASSERT_INPUT(l);
    ASSERT_INPUT(i < l->size);

    list_item_t **t = _rewind_to(l, i);
    list_item_t *f = *t;

    *t = (*t)->next;
    ufree(f);

    l->size--;
}

bool list_contains(const list_t *l, generic_t e)
{
    ASSERT_INPUT(l);

    list_item_t *li = l->head;
    list_item_t *t;
    bool ret = false;

    while (li)
    {
        t = li;
        li = li->next;
        if (generic_compare(t->data, e, l->cmp) == 0)
        {
            ret = true;
            break;
        }
    }

    return ret;
}

void list_reverse(list_t *l)
{
    ASSERT_INPUT(l);
    ABORT("not implemented");
    list_item_t *li = l->head;

    while (li)
    {
    }
}

int list_compare(const list_t *l1, const list_t *l2, void_cmp_t cmp)
{
    ASSERT_INPUT(l1);
    ASSERT_INPUT(l2);
    ASSERT_INPUT(l1->size == l2->size);

    int ret = 0;
    list_item_t *l = l1->head;
    list_item_t *r = l2->head;

    while (l)
    {
        ret = generic_compare(l->data, r->data, cmp);
        if (ret == 0)
        {
            l = l->next;
            r = r->next;
        }
        else
        {
            break;
        }
    }
    return ret;
}

list_t *list_copy(const list_t *l)
{
    ASSERT_INPUT(l);
    return _lcpy(l, false);
}

list_t *list_deep_copy(const list_t *l)
{
    ASSERT_INPUT(l);
    return _lcpy(l, true);
}

int list_print(const list_t *l)
{
    ASSERT_INPUT(l);
    return list_fprint(l, stdout);
}

int list_fprint(const list_t *l, FILE *out)
{
    ASSERT_INPUT(l);
    ASSERT_INPUT(out);

    char *str = list_as_str(l);
    int ret = fprintf(out, "%s\n", str);
    ufree(str);

    return ret;
}

void list_serialize(const list_t *l, buffer_t *buf)
{
    ASSERT_INPUT(l);
    ASSERT_INPUT(buf);

    size_t i = 0;
    list_item_t *li = l->head;
    buffer_append_byte(buf, '[');
    while (li)
    {
        generic_serialize(li->data, buf, l->void_serializer);
        li = li->next;
        if (i++ < l->size - 1)
        {
            buffer_append_data(buf, ", ", 2);
        }
    }
    buffer_append_byte(buf, ']');
}

char *list_as_str(const list_t *l)
{
    ASSERT_INPUT(l);

    buffer_t buf = {0};
    list_serialize(l, &buf);
    buffer_null_terminate(&buf);

    return buf.data;
}

list_iterator_t *list_iterator_create(const list_t *l)
{
    ASSERT_INPUT(l);
    list_iterator_t *li = umalloc(sizeof(*li));

    li->list = l;
    li->next = l->head;

    return li;
}

generic_t list_iterator_get_next(list_iterator_t *li)
{
    ASSERT_INPUT(li);
    ASSERT_MSG(li->list->size, "container is empty");
    ASSERT_MSG(li->next, "iteration is done");

    generic_t g = li->next->data;
    li->next = li->next->next;

    return g;
}

bool list_iterator_has_next(const list_iterator_t *li)
{
    return li->next;
}

void list_iterator_destroy(list_iterator_t *li)
{
    if (li)
    {
        ufree(li);
    }
}

void list_iterator_reset(list_iterator_t *li)
{
    li->next = li->list->head;
}

