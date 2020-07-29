#include "list.h"
#include "mem.h"

/* head->[e{0}]<->[e{1}] ... [e{size-2}]<->[e{size-1}]<-tail */

typedef struct ulist_item {
    ugeneric_t data;
    struct ulist_item *next;
    struct ulist_item *prev;
} ulist_item_t;

struct ulist_opaq {
    uvoid_handlers_t void_handlers;
    bool is_data_owner;
    size_t size;
    ulist_item_t *head;
    ulist_item_t *tail;
};

struct ulist_iterator_opaq {
    const ulist_t *list;
    ulist_item_t *current;
    ulist_item_t fake;
    bool rev;
};

static inline ulist_item_t *_rewind_to(ulist_t *l, size_t i)
{
    ulist_item_t *t = l->head;
    while (i--)
    {
        t = t->next;
    }

    UASSERT_INTERNAL(t);
    return t;
}

static ugeneric_t _pop_at(ulist_t *l, size_t i)
{
    ulist_item_t *t = _rewind_to(l, i);
    ugeneric_t g = t->data;

    if (t == l->head)
    {
        l->head = t->next;
    }
    if (t == l->tail)
    {
        l->tail = t->prev;
    }
    if (t->next)
    {
        t->next->prev = t->prev;
    }
    if (t->prev)
    {
        t->prev->next = t->next;
    }

    ufree(t);
    l->size--;

    return g;
}

static ulist_t *_lcpy(const ulist_t *l, bool deep)
{
    UASSERT_INPUT(l);

    ulist_t *copy = ulist_create();
    *copy = *l;

    ulist_item_t *from = l->head;
    ulist_item_t **to = &copy->head;
    ulist_item_t *prev = NULL;

    copy->is_data_owner = deep;
    while (from)
    {
        *to = umalloc(sizeof(**to));
        (*to)->data = deep ? ugeneric_copy_v(from->data, l->void_handlers.cpy) : from->data;
        (*to)->next = NULL;
        (*to)->prev = prev;
        prev = *to;
        to = &(*to)->next;
        from = from->next;
    }

    copy->tail = prev;

    return copy;
}

ulist_t *ulist_create(void)
{
    ulist_t *l = umalloc(sizeof(*l));

    l->size = 0;
    l->is_data_owner = true;
    l->head = NULL;
    l->tail = NULL;
    memset(&l->void_handlers, 0, sizeof(l->void_handlers));

    return l;
}

void ulist_insert_at(ulist_t *l, size_t i, ugeneric_t e)
{
    UASSERT_INPUT(l);
    UASSERT_INPUT(i < l->size);

    ulist_item_t *li = umalloc(sizeof(*li));
    li->data = e;

    ulist_item_t *t = _rewind_to(l, i);

    // Assume that insert_at is always called on a list with
    // at least one elment so tail pointer is never affected.
    if (t == l->head)
    {
        l->head = li;
    }

    li->next = t;
    li->prev = t->prev;
    if (t->prev)
    {
        t->prev->next = li;
    }
    t->prev = li;

    l->size++;
}

void ulist_append(ulist_t *l, ugeneric_t e)
{
    UASSERT_INPUT(l);

    ulist_item_t *li = umalloc(sizeof(*li));
    li->data = e;
    li->next = NULL;

    if (l->tail)
    {
        li->prev = l->tail;
        li->prev->next = li;
    }
    else
    {
        li->prev = NULL;
        l->head = li;
    }
    l->tail = li;
    l->size++;
}

void ulist_prepend(ulist_t *l, ugeneric_t e)
{
    UASSERT_INPUT(l);

    ulist_item_t *li = umalloc(sizeof(*li));
    li->data = e;
    li->prev = NULL;

    if (l->head)
    {
        li->next = l->head;
        li->next->prev = li;
    }
    else
    {
        li->next = NULL;
        l->tail = li;
    }
    l->head = li;
    l->size++;
}

ugeneric_t ulist_pop_at(ulist_t *l, size_t i)
{
    UASSERT_INPUT(l);
    UASSERT_INPUT(i < l->size);
    return _pop_at(l, i);
}

ugeneric_t ulist_pop_back(ulist_t *l)
{
    UASSERT_INPUT(l);
    UASSERT_INPUT(l->size);
    return _pop_at(l, l->size - 1);
}

ugeneric_t ulist_pop_front(ulist_t *l)
{
    UASSERT_INPUT(l);
    UASSERT_INPUT(l->size);
    return _pop_at(l, 0);
}

void ulist_clear(ulist_t *l)
{
    UASSERT_INPUT(l);

    ulist_item_t *li = l->head;
    ulist_item_t *t;

    while (li)
    {
        if (l->is_data_owner)
        {
            ugeneric_destroy_v(li->data, l->void_handlers.dtr);
        }
        t = li;
        li = li->next;
        ufree(t);
    }
    l->head = NULL;
    l->tail = NULL;
    l->size = 0;
}

void ulist_destroy(ulist_t *l)
{
    if (l)
    {
        ulist_clear(l);
        ufree(l);
    }
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

    ulist_item_t *t = _rewind_to((ulist_t *)l, i);

    return t->data;
}

void ulist_set_at(ulist_t *l, size_t i, ugeneric_t e)
{
    UASSERT_INPUT(l);
    UASSERT_INPUT(i < l->size);

    ulist_item_t *t = _rewind_to(l, i);
    if (l->is_data_owner)
    {
        ugeneric_destroy_v(t->data, l->void_handlers.dtr);
    }
    t->data = e;
}

void ulist_remove_at(ulist_t *l, size_t i)
{
    UASSERT_INPUT(l);
    UASSERT_INPUT(i < l->size);

    ugeneric_t g = _pop_at(l, i);
    if (l->is_data_owner)
    {
        ugeneric_destroy_v(g, l->void_handlers.dtr);
    }
}

void ulist_remove_back(ulist_t *l)
{
    ulist_remove_at(l, l->size - 1);
}

void ulist_remove_front(ulist_t *l)
{
    ulist_remove_at(l, 0);
}

ugeneric_t *ulist_find(ulist_t *l, ugeneric_t e)
{
    UASSERT_INPUT(l);

    ulist_item_t *li = l->head;
    ulist_item_t *t;
    ugeneric_t *g = NULL;

    while (li)
    {
        t = li;
        li = li->next;
        if (ugeneric_compare_v(t->data, e, l->void_handlers.cmp) == 0)
        {
            g = &t->data;
            break;
        }
    }

    return g;
}

bool ulist_contains(const ulist_t *l, ugeneric_t e)
{
    return (ulist_find((ulist_t *)l, e) != NULL);
}

void ulist_reverse(ulist_t *l)
{
    UASSERT_INPUT(l);

    if (l->size > 1)
    {
        ulist_item_t *t = l->head;
        ulist_item_t *li;

        while (t)
        {
            li = t;
            t = li->next;
            li->next = li->prev;
            li->prev = t;
        }

        t = l->head;
        l->head = l->tail;
        l->tail = t;
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
        int diff = ugeneric_compare_v(l->data, r->data, cmp);
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

    return (l1->size > l2->size) - (l1->size < l2->size);
}

ulist_t *ulist_copy(const ulist_t *l)
{
    return _lcpy(l, false);
}

ulist_t *ulist_deep_copy(const ulist_t *l)
{
    return _lcpy(l, true);
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

static void _iterator_reset(ulist_iterator_t *li)
{
    if (li->rev)
    {
        li->fake.next = NULL;
        li->fake.prev = li->list->tail;
    }
    else
    {
        li->fake.next = li->list->head;
        li->fake.prev = NULL;
    }

    li->current = &li->fake;
}

static ulist_iterator_t *_iterator_create(const ulist_t *l, bool rev)
{
    UASSERT_INPUT(l);
    ulist_iterator_t *li = umalloc(sizeof(*li));

    li->list = l;
    li->rev = rev;

    _iterator_reset(li);

    return li;
}

ugeneric_t *_iterator_get_next(ulist_iterator_t *li)
{
    UASSERT_INPUT(li);
    UASSERT_MSG(li->list->size, "container is empty");

    if (li->rev)
    {
        UASSERT_MSG(li->current->prev, "no next");
        li->current = li->current->prev;
    }
    else
    {
        UASSERT_MSG(li->current->next, "no next");
        li->current = li->current->next;
    }

    return &li->current->data;
}

ugeneric_t *_iterator_get_prev(ulist_iterator_t *li)
{
    UASSERT_INPUT(li);
    UASSERT_MSG(li->list->size, "container is empty");

    if (li->rev)
    {
        UASSERT_MSG(li->current->next, "no prev");
        li->current = li->current->next;
    }
    else
    {
        UASSERT_MSG(li->current->prev, "no prev");
        li->current = li->current->prev;
    }

    return &li->current->data;
}

ulist_iterator_t *ulist_iterator_create_rev(const ulist_t *l)
{
    UASSERT_INPUT(l);
    return _iterator_create(l, true);
}

ulist_iterator_t *ulist_iterator_create(const ulist_t *l)
{
    UASSERT_INPUT(l);
    return _iterator_create(l, false);
}

ugeneric_t ulist_iterator_get_next(ulist_iterator_t *li)
{
    return *_iterator_get_next(li);
}

ugeneric_t *ulist_iterator_get_next_ref(ulist_iterator_t *li)
{
    return _iterator_get_next(li);
}

ugeneric_t ulist_iterator_get_prev(ulist_iterator_t *li)
{
    UASSERT_INPUT(li);
    return *_iterator_get_prev(li);
}

ugeneric_t *ulist_iterator_get_prev_ref(ulist_iterator_t *li)
{
    UASSERT_INPUT(li);
    return _iterator_get_prev(li);
}

bool ulist_iterator_has_next(const ulist_iterator_t *li)
{
    UASSERT_INPUT(li);

    if (li->rev)
    {
        return li->list->size && li->current->prev;
    }
    else
    {
        return li->list->size && li->current->next;
    }
}

bool ulist_iterator_has_prev(const ulist_iterator_t *li)
{
    UASSERT_INPUT(li);

    if (li->rev)
    {
        return li->list->size && li->current->next;
    }
    else
    {
        return li->list->size && li->current->prev;
    }
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
    UASSERT_INPUT(li);
    _iterator_reset(li);
}

ugeneric_base_t *ulist_get_base(ulist_t *l)
{
    UASSERT_INPUT(l);
    return (ugeneric_base_t *)l;
}
