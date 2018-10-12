#include "htbl.h"

#include "asserts.h"
#include "mem.h"

#define UHTBL_INITIAL_NUM_OF_BUCKETS 4
#define UHTBL_C_LOAD_THRESHOLD 0.75
#define UHTBL_OA_LOAD_THRESHOLD 0.5

static uhtbl_type_t _default_type = UHTBL_TYPE_CHAINING;

// Hack around internal types, G_NULL always contains 0 in value part
#define _IS_EMPTY(x)         (G_IS_NULL((x)->k) && ((x)->k.v.integer == 0x01))
#define _IS_TOMBSTONE(x)     (G_IS_NULL((x)->k) && ((x)->k.v.integer == 0x02))
#define _SET_TO_EMPTY(x)     ((x)->k = G_NULL(), (x)->k.v.integer = 0x01)
#define _SET_TO_TOMBSTONE(x) ((x)->k = G_NULL(), (x)->k.v.integer = 0x02)

struct uhtbl_record {
    ugeneric_kv_t kv;
    struct uhtbl_record *next;
};
typedef struct uhtbl_record uhtbl_record_t;

typedef struct {
    void (*destroy_buckets)(uhtbl_t *h);
    void (*resize)(uhtbl_t *h);
    void (*put)(uhtbl_t *h, ugeneric_t k, ugeneric_t v);
    bool (*pop)(uhtbl_t *h, ugeneric_t k, ugeneric_t *out);
    ugeneric_kv_t *(*find_kv)(const uhtbl_t *h, ugeneric_t k);
    float load_threshold;
} uhtbl_vtable_t;

struct uhtbl_opaq {
    uvoid_handlers_t void_handlers;
    bool is_data_owner;
    uhtbl_type_t type;
    union {
        uhtbl_record_t **c_buckets; // chaining
        ugeneric_kv_t *oa_buckets;  // open-addressing
    };
    size_t number_of_records;
    size_t number_of_buckets;
    size_t number_of_occupied_buckets;
    void_hasher_t hasher;
    void_cmp_t key_cmp;
    const uhtbl_vtable_t *vtable;
};

struct uhtbl_iterator_opaq {
    const uhtbl_t *htbl;
    uhtbl_record_t *current_dr;
    size_t bucket;
    size_t records_to_iterate;
};

static void _oa_destroy_buckets(uhtbl_t *h);
static void _oa_put(uhtbl_t *h, ugeneric_t k, ugeneric_t v);
static bool _oa_pop(uhtbl_t *h, ugeneric_t k, ugeneric_t *out);
static ugeneric_kv_t *_oa_find_kv(const uhtbl_t *h, ugeneric_t k);

static void _c_destroy_buckets(uhtbl_t *h);
static void _c_put(uhtbl_t *h, ugeneric_t k, ugeneric_t v);
static bool _c_pop(uhtbl_t *h, ugeneric_t k, ugeneric_t *out);
static ugeneric_kv_t *_c_find_kv(const uhtbl_t *h, ugeneric_t k);

// Collision addressing with open addressing.
static const uhtbl_vtable_t _uhtbl_oa_table = {
    .destroy_buckets = _oa_destroy_buckets,
    .put = _oa_put,
    .pop = _oa_pop,
    .find_kv = _oa_find_kv,
    .load_threshold = UHTBL_OA_LOAD_THRESHOLD,
};

// Collision addressing with chaining.
static const uhtbl_vtable_t _uhtbl_c_table = {
    .destroy_buckets = _c_destroy_buckets,
    .put = _c_put,
    .pop = _c_pop,
    .find_kv = _c_find_kv,
    .load_threshold = UHTBL_C_LOAD_THRESHOLD,
};

static ugeneric_kv_t *_oa_allocate_buckets(size_t count)
{
    ugeneric_kv_t *buckets = umalloc(count * sizeof(*buckets));
    for (size_t i = 0; i < count; i++)
    {
        _SET_TO_EMPTY(buckets + i);
    }

    return buckets;
}

static void _oa_destroy_buckets(uhtbl_t *h)
{
    for (size_t i = 0; i < h->number_of_buckets; i++)
    {
        ugeneric_kv_t *kv = &h->oa_buckets[i];
        if (h->is_data_owner && !_IS_EMPTY(kv) && !_IS_TOMBSTONE(kv))
        {
            ugeneric_destroy_v(kv->k, h->void_handlers.dtr);
            ugeneric_destroy_v(kv->v, h->void_handlers.dtr);
        }
        _SET_TO_EMPTY(h->oa_buckets + i);
    }
}

static void _c_destroy_buckets(uhtbl_t *h)
{
    for (size_t i = 0; i < h->number_of_buckets; i++)
    {
        uhtbl_record_t *hr = h->c_buckets[i];
        while (hr)
        {
            uhtbl_record_t *hr_next = hr->next;
            if (h->is_data_owner)
            {
                ugeneric_destroy_v(hr->kv.k, h->void_handlers.dtr);
                ugeneric_destroy_v(hr->kv.v, h->void_handlers.dtr);
            }
            ufree(hr);
            hr = hr_next;
        }
        h->c_buckets[i] = NULL;
    }
}

/*
 * Either a record pointer or NULL when we are at the end of table.
 */
static uhtbl_record_t *_c_find_next_record(const uhtbl_t *h,
                                           uhtbl_record_t *current_dr,
                                           size_t *bucket)
{
    uhtbl_record_t *next_record = NULL;

    if (current_dr)
    {
        next_record = current_dr->next;
        if (!next_record)
        {
            *bucket += 1;
        }
    }

    while (!next_record && (*bucket < h->number_of_buckets))
    {
        next_record = h->c_buckets[*bucket];
        if (!next_record)
        {
            *bucket += 1;
        }
    }

    return next_record;
}

static ugeneric_kv_t *_oa_find_next_kv(const uhtbl_t *h, size_t *bucket)
{
    ugeneric_kv_t *kv = NULL;
    while (*bucket < h->number_of_buckets)
    {
        ugeneric_kv_t *t = &h->oa_buckets[*bucket];
        *bucket += 1;
        if (!_IS_EMPTY(t) && !_IS_TOMBSTONE(t))
        {
            kv = t;
            break;
        }
    }

    UASSERT_INTERNAL(kv);

    return kv;
}

/*
 * Return either a pointer to corresponded htbl record found by the key
 * or a pointer to the place where the record should be placed.
 */
static uhtbl_record_t **_c_find_record(const uhtbl_t *h, ugeneric_t k)
{
    uhtbl_record_t **hr;
    hr = &h->c_buckets[ugeneric_hash(k, h->hasher) % h->number_of_buckets];
    while (*hr)
    {
        if (ugeneric_compare_v((*hr)->kv.k, k, h->key_cmp) == 0)
        {
            break;
        }
        hr = &(*hr)->next;
    }

    return hr;
}

static ugeneric_kv_t *_c_find_kv(const uhtbl_t *h, ugeneric_t k)
{
    uhtbl_record_t **hr = _c_find_record(h, k);
    return (*hr) ? &(*hr)->kv : NULL;
}

/*
 * Return either pointer to corresponded slot found by key
 * or a pointer to place where such a record should be placed.
 */
static ugeneric_kv_t *_oa_find_slot(const uhtbl_t *h, ugeneric_t k)
{
    size_t i = 0;
    ugeneric_kv_t *ret = NULL;
    size_t bucket = ugeneric_hash(k, h->hasher) % h->number_of_buckets;

    while (i < h->number_of_buckets)
    {
        ugeneric_kv_t *kv = &h->oa_buckets[bucket];
        if (_IS_EMPTY(kv))
        {
            // This is the place where to put the data (update case)
            // or just indication that data for requested key is
            // not present (lookup case).
            if (!ret)
            {
                ret = kv;
            }
            break;
        }
        else if (_IS_TOMBSTONE(kv))
        {
            // Remember this bucket and keep going until an empty slot is
            // found (insert position) or existing data for requested key
            // is found (update position).
            ret = kv;
        }
        else
        {
            if (ugeneric_compare_v(kv->k, k, h->key_cmp) == 0)
            {
                return kv;
            }
        }
        bucket += 1;
        bucket %= h->number_of_buckets;
        i++;
    }

    if (ret)
    {
        return ret;
    }

    UABORT("internal error");
}

/* Return either pointer to record found by key or NULL */
static ugeneric_kv_t *_oa_find_kv(const uhtbl_t *h, ugeneric_t k)
{
    ugeneric_kv_t *kv = _oa_find_slot(h, k);
    if (_IS_EMPTY(kv) || _IS_TOMBSTONE(kv))
    {
        kv = NULL;
    }

    return kv;
}

static float _get_load_factor(const uhtbl_t *h)
{
    switch (h->type)
    {
        case UHTBL_TYPE_CHAINING:
            return (float)h->number_of_records / h->number_of_buckets;
        case UHTBL_TYPE_OPEN_ADDRESSING:
            return (float)h->number_of_occupied_buckets / h->number_of_buckets;
        default:
            UABORT("internal error");
    }
}

static void _replace_kv(uhtbl_t *h, ugeneric_kv_t *kv,
                        ugeneric_t k, ugeneric_t v)
{
    if (h->is_data_owner)
    {
        ugeneric_destroy_v(kv->k, h->void_handlers.dtr);
        ugeneric_destroy_v(kv->v, h->void_handlers.dtr);
    }
    kv->k = k;
    kv->v = v;
}

static void _oa_put(uhtbl_t *h, ugeneric_t k, ugeneric_t v)
{
    ugeneric_kv_t *kv = _oa_find_slot(h, k);
    if (_IS_EMPTY(kv) || _IS_TOMBSTONE(kv))
    {
        kv->k = k;
        kv->v = v;
        h->number_of_records += 1;
        if (!_IS_TOMBSTONE(kv))
        {
            h->number_of_occupied_buckets += 1;
        }
    }
    else
    {
        _replace_kv(h, kv, k, v);
    }
}

static void _c_put(uhtbl_t *h, ugeneric_t k, ugeneric_t v)
{
    uhtbl_record_t **hr = _c_find_record(h, k);

    if (*hr)
    {
        // Update existing.
        _replace_kv(h, &(*hr)->kv, k, v);
    }
    else
    {
        // Insert a new one.
        *hr = umalloc(sizeof(uhtbl_record_t));
        (*hr)->kv.k = k;
        (*hr)->kv.v = v;
        (*hr)->next = NULL;
        h->number_of_records += 1;
    }
}

static bool _oa_pop(uhtbl_t *h, ugeneric_t k, ugeneric_t *out)
{
    bool ret = false;
    ugeneric_kv_t *kv = _oa_find_kv(h, k);

    if (kv)
    {
        ugeneric_destroy_v(kv->k, h->void_handlers.dtr);
        *out = kv->v;
        h->number_of_records -= 1;
        _SET_TO_TOMBSTONE(kv);
        ret = true;
    }

    return ret;
}

static bool _c_pop(uhtbl_t *h, ugeneric_t k, ugeneric_t *out)
{
    bool ret = false;
    uhtbl_record_t **hr = _c_find_record(h, k);

    if (*hr)
    {
        uhtbl_record_t *del = *hr;
        *out = del->kv.v;
        ugeneric_destroy_v(del->kv.k, h->void_handlers.dtr);
        *hr = (*hr)->next;
        ufree(del);
        h->number_of_records -= 1;
        ret = true;
    }

    return ret;
}

static void _resize(uhtbl_t *h)
{
    uhtbl_t new_table;

    memcpy(&new_table, h, sizeof(*h));
    new_table.number_of_records = 0;
    new_table.number_of_occupied_buckets = 0;
    new_table.number_of_buckets = SCALE_FACTOR * h->number_of_buckets;
    switch (h->type)
    {
        case UHTBL_TYPE_CHAINING:
            new_table.oa_buckets = ucalloc(new_table.number_of_buckets,
                                           sizeof(new_table.c_buckets[0]));
            for (size_t i = 0; i < h->number_of_buckets; i++)
            {
                uhtbl_record_t *hr = h->c_buckets[i];
                while (hr)
                {
                    uhtbl_record_t *t = hr->next;
                    _c_put(&new_table, hr->kv.k, hr->kv.v);
                    ufree(hr);
                    hr = t;
                }
            }
            ufree(h->c_buckets);
            break;
        case UHTBL_TYPE_OPEN_ADDRESSING:
            new_table.oa_buckets = _oa_allocate_buckets(new_table.number_of_buckets);
            for (size_t i = 0; i < h->number_of_buckets; i++)
            {
                ugeneric_kv_t *kv = &h->oa_buckets[i];
                if (!_IS_EMPTY(kv) && !_IS_TOMBSTONE(kv))
                {
                    _oa_put(&new_table, kv->k, kv->v);
                }
            }
            ufree(h->oa_buckets);
            break;
        default:
            UABORT("internal error");
    }

    UASSERT_INTERNAL(new_table.number_of_records == h->number_of_records);
    memcpy(h, &new_table, sizeof(*h));
}

uhtbl_t *uhtbl_create(void)
{
    return uhtbl_create_with_type(UHTBL_TYPE_DEFAULT);
}

uhtbl_t *uhtbl_create_with_type(uhtbl_type_t type)
{
    UASSERT_INPUT(type >= UHTBL_TYPE_DEFAULT);
    UASSERT_INPUT(type < UHTBL_TYPE_MAX);

    uhtbl_t *h = umalloc(sizeof(*h));

    h->type = (type == UHTBL_TYPE_DEFAULT) ? _default_type : type;

    switch (h->type)
    {
        case UHTBL_TYPE_CHAINING:
            h->vtable = &_uhtbl_c_table;
            h->c_buckets = ucalloc(UHTBL_INITIAL_NUM_OF_BUCKETS, sizeof(h->c_buckets[0]));
            break;
        case UHTBL_TYPE_OPEN_ADDRESSING:
            h->vtable = &_uhtbl_oa_table;
            h->oa_buckets = _oa_allocate_buckets(UHTBL_INITIAL_NUM_OF_BUCKETS);
            break;
        default:
            UABORT("internal error");
    }

    h->number_of_records = 0;
    h->number_of_buckets = UHTBL_INITIAL_NUM_OF_BUCKETS;
    h->number_of_occupied_buckets = 0;
    memset(&h->void_handlers, 0, sizeof(h->void_handlers));
    h->is_data_owner = true;
    h->hasher = NULL;
    h->key_cmp = NULL;

    return h;
}

void uhtbl_set_void_key_comparator(uhtbl_t *h, void_cmp_t cmp)
{
    UASSERT_INPUT(h);
    h->key_cmp = cmp;
}

void_cmp_t uhtbl_get_void_key_comparator(const uhtbl_t *h)
{
    UASSERT_INPUT(h);
    return h->key_cmp;
}

void uhtbl_set_void_hasher(uhtbl_t *h, void_hasher_t hasher)
{
    UASSERT_INPUT(h);
    h->hasher = hasher;
}

void_hasher_t uhtbl_get_void_hasher(const uhtbl_t *h)
{
    UASSERT_INPUT(h);
    return h->hasher;
}

/*
 * Puts without copy, keys and values which contain pointers
 * may cause issue if you forget who owns the data.
 */
void uhtbl_put(uhtbl_t *h, ugeneric_t k, ugeneric_t v)
{
    UASSERT_INPUT(h);

    h->vtable->put(h, k, v);

    if (_get_load_factor(h) >= h->vtable->load_threshold)
    {
       _resize(h);
    }
}

/* Returns either data stored in htbl or vdef if data is not,
 * found by the key; data remains in the container.
*/
ugeneric_t uhtbl_get(const uhtbl_t *h, ugeneric_t k, ugeneric_t vdef)
{
    UASSERT_INPUT(h);
    const ugeneric_kv_t *kv = h->vtable->find_kv(h, k);
    return kv ? kv->v : vdef;
}

/* Returns either data stored in htbl or vdef if data is not
 * found by the key; data is popped out from container.
*/
ugeneric_t uhtbl_pop(uhtbl_t *h, ugeneric_t k, ugeneric_t vdef)
{
    UASSERT_INPUT(h);
    h->vtable->pop(h, k, &vdef);
    return vdef;
}

bool uhtbl_remove(uhtbl_t *h, ugeneric_t k)
{
    UASSERT_INPUT(h);

    ugeneric_t v;
    bool ret = h->vtable->pop(h, k, &v);
    if (ret && h->is_data_owner)
    {
        ugeneric_destroy_v(v, h->void_handlers.dtr);
    }

    return ret;
}

void uhtbl_destroy(uhtbl_t *h)
{
    if (h)
    {
        h->vtable->destroy_buckets(h);
        switch (h->type)
        {
            case UHTBL_TYPE_CHAINING:
                ufree(h->c_buckets);
                break;
            case UHTBL_TYPE_OPEN_ADDRESSING:
                ufree(h->oa_buckets);
                break;
            default:
                UABORT("internal error");
        }
        ufree(h);
    }
}

void uhtbl_clear(uhtbl_t *h)
{
    UASSERT_INPUT(h);
    h->vtable->destroy_buckets(h);
    h->number_of_records = 0;
    h->number_of_occupied_buckets = 0;
}

void uhtbl_serialize(const uhtbl_t *h, ubuffer_t *buf)
{
    UASSERT_INPUT(h);
    UASSERT_INPUT(buf);

    ubuffer_append_byte(buf, '{');
    uhtbl_iterator_t *hi = uhtbl_iterator_create(h);
    while (uhtbl_iterator_has_next(hi))
    {
        ugeneric_kv_t kv = uhtbl_iterator_get_next(hi);
        ugeneric_serialize_v(kv.k, buf, h->void_handlers.s8r);
        ubuffer_append_data(buf, ": ", 2);
        ugeneric_serialize_v(kv.v, buf, h->void_handlers.s8r);
        if (uhtbl_iterator_has_next(hi))
        {
            ubuffer_append_data(buf, ", ", 2);
        }
    }
    uhtbl_iterator_destroy(hi);
    ubuffer_append_byte(buf, '}');
}

char *uhtbl_as_str(const uhtbl_t *h)
{
    UASSERT_INPUT(h);

    ubuffer_t buf = {0};
    uhtbl_serialize(h, &buf);
    ubuffer_null_terminate(&buf);

    return buf.data;
}

int uhtbl_fprint(const uhtbl_t *h, FILE *out)
{
    UASSERT_INPUT(h);
    UASSERT_INPUT(out);

    char *str = uhtbl_as_str(h);
    int ret = fprintf(out, "%s\n", str);
    ufree(str);

    return ret;
}

void uhtbl_dump_to_dot(const uhtbl_t *h, FILE *out)
{
    UASSERT_INPUT(h);
    UASSERT_INPUT(out);

    fprintf(out, "%s %s {\n", "digraph", "uhtbl_name");
    fprintf(out, "\trankdir=LR;\n");
    fprintf(out, "\tnode [shape=record];\n");

    size_t j = 1;
    if (h->type == UHTBL_TYPE_CHAINING)
    {
        // buckets
        fprintf(out, "\tnode0 [label = \"");
        for (size_t i = 0; i < h->number_of_buckets; i++)
        {
            fprintf(out, "%s<f%zu>", i ? "|" : "", i);
        }
        fprintf(out, "\"];");

        for (size_t i = 0; i < h->number_of_buckets; i++)
        {
            uhtbl_record_t *hr = h->c_buckets[i];
            if (hr)
            {
                size_t k = j;
                fprintf(out, "\n");
                while (hr)
                {
                    char *k = ugeneric_as_str_v(hr->kv.k, NULL);
                    char *v = ugeneric_as_str_v(hr->kv.v, NULL);
                    fprintf(out, "\tnode%zu [label = \"{ <data> %s:%s | <ref> }\"];\n",
                            j++, k, v);
                    hr = hr->next;
                    ufree(k);
                    ufree(v);
                }
                fprintf(out, "\tnode0:f%zu -> node%zu:data;\n", i, k);
                while (k < (j - 1))
                {
                    fprintf(out, "\tnode%zu:ref -> node%zu:data;\n", k, k + 1);
                    k++;
                }
            }
        }
    }
    else if (h->type == UHTBL_TYPE_OPEN_ADDRESSING)
    {
        fprintf(out, "\tnode0 [label = \"");
        for (size_t i = 0; i < h->number_of_buckets; i++)
        {
            ugeneric_kv_t *kv = &h->oa_buckets[i];
            if (_IS_EMPTY(kv))
            {
                fprintf(out, "%sempty", i ? "|" : "");
            }
            else if _IS_TOMBSTONE(kv)
            {
                fprintf(out, "%sRIP", i ? "|" : "");
            }
            else
            {
                char *k = ugeneric_as_str_v(kv->k, NULL);
                char *v = ugeneric_as_str_v(kv->v, NULL);
                fprintf(out, "%s%s:%s", i ? "|" : "", k, v);
                ufree(k);
                ufree(v);
            }
        }
        fprintf(out, "\"];\n");
    }
    else
    {
        UABORT("internal error");
    }

    fprintf(out, "}\n");
}

uhtbl_iterator_t *uhtbl_iterator_create(const uhtbl_t *h)
{
    UASSERT_INPUT(h);

    uhtbl_iterator_t *hi = umalloc(sizeof(*hi));
    hi->htbl = h;
    hi->bucket = 0;
    hi->records_to_iterate = h->number_of_records;
    hi->current_dr = NULL;

    return hi;
}

ugeneric_kv_t uhtbl_iterator_get_next(uhtbl_iterator_t *hi)
{
    UASSERT_INPUT(hi);
    UASSERT_MSG(hi->records_to_iterate, "iteration is done");
    UASSERT_MSG(hi->htbl->number_of_records, "container is empty");

    ugeneric_kv_t *kv = NULL;
    hi->records_to_iterate -= 1;

    switch (hi->htbl->type)
    {
        case UHTBL_TYPE_CHAINING:
            hi->current_dr = _c_find_next_record(hi->htbl, hi->current_dr, &hi->bucket);
            kv = &hi->current_dr->kv;
            break;
        case UHTBL_TYPE_OPEN_ADDRESSING:
            kv = _oa_find_next_kv(hi->htbl, &hi->bucket);
            break;
        default:
            UABORT("internal error");
    }

    return *kv;
}

bool uhtbl_iterator_has_next(const uhtbl_iterator_t *hi)
{
    UASSERT_INPUT(hi);
    return hi->records_to_iterate;
}

void uhtbl_iterator_reset(uhtbl_iterator_t *hi)
{
    UASSERT_INPUT(hi);
    hi->bucket = 0;
    hi->current_dr = NULL;
    hi->records_to_iterate = hi->htbl->number_of_records;
}

void uhtbl_iterator_destroy(uhtbl_iterator_t *hi)
{
    if (hi)
    {
        ufree(hi);
    }
}

size_t uhtbl_get_size(const uhtbl_t *h)
{
    UASSERT_INPUT(h);
    return h->number_of_records;
}

bool uhtbl_is_empty(const uhtbl_t *h)
{
    UASSERT_INPUT(h);
    return h->number_of_records == 0;
}

bool uhtbl_has_key(const uhtbl_t *h, ugeneric_t k)
{
    UASSERT_INPUT(h);

    ugeneric_kv_t *kv = h->vtable->find_kv(h, k);
    return kv != NULL;
}

uvector_t *uhtbl_get_items(const uhtbl_t *h, udict_items_kind_t kind, bool deep)
{
    UASSERT_INPUT(h);
    (void)deep; //TODO: implement it

    uhtbl_iterator_t *hi = uhtbl_iterator_create(h);
    uvector_t *v = uvector_create();
    uvector_reserve_capacity(v, h->number_of_records);
    while (uhtbl_iterator_has_next(hi))
    {
        ugeneric_kv_t item = uhtbl_iterator_get_next(hi);
        switch (kind)
        {
            case UDICT_KEYS:
                uvector_append(v, item.k);
                break;
            case UDICT_VALUES:
                uvector_append(v, item.v);
                break;
            case UDICT_KV:
                uvector_append(v, item.k);
                uvector_append(v, item.v);
                break;
            default:
                UABORT("internal error");
        }
    }
    uhtbl_iterator_destroy(hi);

    uvector_drop_data_ownership(v);
    uvector_set_void_comparator(v, h->void_handlers.cmp); // vector sort should use original cmp
    uvector_set_void_serializer(v, h->void_handlers.s8r);
    uvector_shrink_to_size(v);

    return v;
}

ugeneric_base_t *uhtbl_get_base(uhtbl_t *h)
{
    UASSERT_INPUT(h);
    return (ugeneric_base_t *)h;
}
