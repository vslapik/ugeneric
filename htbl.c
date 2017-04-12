#include <stdlib.h>
#include <string.h>
#include "mem.h"
#include "htbl.h"

#define HTBL_SCALE_FACTOR 2
#define HTBL_INITIAL_NUM_OF_BUCKETS 32
#define REHASH_THRESHOLD (3.0)/(4.0)

struct htbl_record {
    generic_t k;
    generic_t v;
    struct htbl_record *next;
};
typedef struct htbl_record htbl_record_t;

struct htbl_opaq {
    htbl_record_t **buckets;
    size_t number_of_buckets;
    size_t number_of_records;
    bool is_data_owner;
    void_hasher_t hasher;
    void_cmp_t key_cmp;
    void_cpy_t cpy;
    void_cmp_t cmp;
    void_dtr_t dtr;
};

struct htbl_iterator_opaq {
    const htbl_t *htbl;
    htbl_record_t *current_dr;
    size_t bucket;
    size_t records_to_iterate;
};

static void _destroy_buckets(htbl_t *h)
{
    for (size_t i = 0; i < h->number_of_buckets; i++)
    {
        htbl_record_t *hr = h->buckets[i];
        while (hr)
        {
            htbl_record_t *hr_next = hr->next;
            if (h->is_data_owner)
            {
                generic_destroy(hr->k, h->dtr);
                generic_destroy(hr->v, h->dtr);
            }
            ufree(hr);
            hr = hr_next;
        }
    }
}

/*
 * Either a record pointer or NULL when we are at the end of table.
 */
static htbl_record_t *_find_next_record(const htbl_t *h,
                                        htbl_record_t *current_dr,
                                        size_t *bucket)
{
    htbl_record_t *next_record = NULL;

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
        next_record = h->buckets[*bucket];
        if (!next_record)
        {
            *bucket += 1;
        }
    }

    return next_record;
}

/*
 * Either return pointer to corresponded htbl record found by key
 * or return pointer to place where such a record should be placed.
 */
static htbl_record_t **_find_by_key(const htbl_t *h, generic_t k)
{
    htbl_record_t **hr;
    hr = &h->buckets[generic_hash(k, h->hasher) % h->number_of_buckets];
    while (*hr)
    {
        if (generic_compare((*hr)->k, k, h->key_cmp) == 0)
        {
            break;
        }
        hr = &(*hr)->next;
    }

    return hr;
}

static float _get_load_factor(const htbl_t *h)
{
    return (float)h->number_of_records / h->number_of_buckets;
}
/*
static void _rehash(htbl_t *h)
{
    size_t new_number_of_buckets;
    htbl_record_t **new_buckets;

    new_number_of_buckets = HTBL_SCALE_FACTOR * d->number_of_buckets;
    new_buckets = ucalloc(new_number_of_buckets,
                          sizeof(d->buckets[0]));

    for (size_t i = 0; i < d->number_of_buckets; i++)
    {
        htbl_record_t *hr = d->buckets[i];
        while (dr)
        {
            htbl_record_t *hr_next = dr->next;
            if (d->is_data_owner)
            {
                generic_destroy(dr->k, d->dtr);
                generic_destroy(dr->v, d->dtr);
            }
            ufree(dr);
            dr = dr_next;
        }
    }

    //_destroy_buckets(

    d->buckets = new_buckets;
}
*/

htbl_t *htbl_create(void)
{
    htbl_t *h = umalloc(sizeof(*h));
    h->buckets = ucalloc(HTBL_INITIAL_NUM_OF_BUCKETS,
                         sizeof(h->buckets[0]));
    h->number_of_buckets = HTBL_INITIAL_NUM_OF_BUCKETS;
    h->number_of_records = 0;
    h->is_data_owner = true;
    h->hasher = NULL;
    h->key_cmp = NULL;
    h->cpy = NULL;
    h->cmp = NULL;
    h->dtr = NULL;

    return h;
}

void htbl_take_data_ownership(htbl_t *h)
{
    ASSERT_INPUT(h);
    h->is_data_owner = true;
}

void htbl_drop_data_ownership(htbl_t *h)
{
    ASSERT_INPUT(h);
    h->is_data_owner = false;
}

void htbl_set_destroyer(htbl_t *h, void_dtr_t dtr)
{
    ASSERT_INPUT(h);
    h->dtr = dtr;
}

void htbl_set_comparator(htbl_t *h, void_cmp_t cmp)
{
    ASSERT_INPUT(h);
    h->cmp = cmp;
}

void htbl_set_hasher(htbl_t *h, void_hasher_t hasher)
{
    ASSERT_INPUT(h);
    h->hasher = hasher;
}

/*
 * Puts without copy, keys and values which contains pointers
 * may cause issue if you forget who owns the data.
 */
void htbl_put(htbl_t *h, generic_t k, generic_t v)
{
    ASSERT_INPUT(h);
    htbl_record_t **hr = _find_by_key(h, k);

    if (_get_load_factor(h) >= REHASH_THRESHOLD)
    {
        //_rehash(d);
        // todo: implement me
    }

    if (*hr)
    {
        // Update existing.
        if (h->is_data_owner)
        {
            generic_destroy((*hr)->k, h->dtr);
            generic_destroy((*hr)->v, h->dtr);
        }
        (*hr)->k = k;
        (*hr)->v = v;
    }
    else
    {
        // Insert a new one.
        *hr = umalloc(sizeof(htbl_record_t));
        (*hr)->k = k;
        (*hr)->v = v;
        (*hr)->next = NULL;
        h->number_of_records += 1;
    }
}

/* Returns either data stored in htbl or
 * vdef if data is not found by the key.
*/
generic_t htbl_get(const htbl_t *h, generic_t k, generic_t vdef)
{
    ASSERT_INPUT(h);
    htbl_record_t **hr = _find_by_key(h, k);

    return (*hr) ? (*hr)->v : vdef;
}

generic_t htbl_pop(htbl_t *h, generic_t k, generic_t vdef)
{
    ASSERT_INPUT(h);
    generic_t ret = vdef;
    htbl_record_t **hr = _find_by_key(h, k);

    if (*hr)
    {
        htbl_record_t *del = *hr;
        ret = del->v;
        generic_destroy(del->k, h->dtr);
        *hr = (*hr)->next;
        ufree(del);
        h->number_of_records -= 1;
    }

    return ret;
}

void htbl_destroy(htbl_t *h)
{
    if (h)
    {
        _destroy_buckets(h);
        ufree(h->buckets);
        ufree(h);
    }
}

void htbl_clear(htbl_t *h)
{
    ASSERT_INPUT(h);
    _destroy_buckets(h);
    memset(h->buckets, 0, h->number_of_buckets * sizeof(h->buckets[0]));
    h->number_of_records = 0;
}

void htbl_serialize(const htbl_t *h, buffer_t *buf)
{
    ASSERT_INPUT(h);
    ASSERT_INPUT(buf);

    buffer_append_byte(buf, '{');
    htbl_iterator_t *hi = htbl_iterator_create(h);
    while (htbl_iterator_has_next(hi))
    {
        generic_kv_t kv = htbl_iterator_get_next(hi);
        generic_serialize(kv.k, buf);
        buffer_append_data(buf, ": ", 2);
        generic_serialize(kv.v, buf);
        if (htbl_iterator_has_next(hi))
        {
            buffer_append_data(buf, ", ", 2);
        }
    }
    htbl_iterator_destroy(hi);
    buffer_append_byte(buf, '}');
}

char *htbl_as_str(const htbl_t *h)
{
    ASSERT_INPUT(h);

    buffer_t buf = {0};
    htbl_serialize(h, &buf);
    buffer_null_terminate(&buf);

    return buf.data;
}

int htbl_print(const htbl_t *h)
{
    ASSERT_INPUT(h);
    return htbl_fprint(h, stdout);
}

int htbl_fprint(const htbl_t *h, FILE *out)
{
    ASSERT_INPUT(h);
    ASSERT_INPUT(out);

    char *str = htbl_as_str(h);
    int ret = fprintf(out, "%s\n", str);
    ufree(str);

    return ret;
}

void htbl_dump_to_dot(const htbl_t *h, FILE *out)
{
    ASSERT_INPUT(h);
    ASSERT_INPUT(out);

    fprintf(out, "%s %s {\n", "higraph", "htbl_name");
    fprintf(out, "    rankhir=LR;\n");
    fprintf(out, "    node [shape=record];\n");

    // buckets
    fprintf(out, "    node0 [label = \"");
    for (size_t i = 0; i < h->number_of_buckets; i++)
    {
        fprintf(out, "<f%zu>|", i);
    }
    fprintf(out, "\"];\n");

    size_t j = 1;
    for (size_t i = 0; i < h->number_of_buckets; i++)
    {
        htbl_record_t *hr = h->buckets[i];
        if (hr)
        {
            size_t k = j;
            fprintf(out, "\n");
            while (hr)
            {
                char *k = generic_as_str(hr->k);
                char *v = generic_as_str(hr->v);
                fprintf(out, "    node%zu [label = \"{ <data> '%s':'%s' | <ref> }\"];\n",
                        j++, k, v);
                hr = hr->next;
                ufree(k);
                ufree(v);
            }
            fprintf(out, "    node0:f%zu -> node%zu:data;\n", i, k);
            while (k < (j - 1))
            {
                fprintf(out, "    node%zu:ref -> node%zu:data;\n", k, k + 1);
                k++;
            }
        }
    }

    fprintf(out, "}\n");
}

htbl_iterator_t *htbl_iterator_create(const htbl_t *h)
{
    ASSERT_INPUT(h);

    htbl_iterator_t *hi = umalloc(sizeof(*hi));
    hi->htbl = h;
    hi->bucket = 0;
    hi->records_to_iterate = h->number_of_records;
    hi->current_dr = NULL;

    return hi;
}

generic_kv_t htbl_iterator_get_next(htbl_iterator_t *hi)
{
    ASSERT_INPUT(hi);
    ASSERT_MSG(hi->records_to_iterate, "iteration is done");
    ASSERT_MSG(hi->htbl->number_of_records, "container is empty");

    hi->current_dr = _find_next_record(hi->htbl, hi->current_dr, &hi->bucket);
    generic_kv_t kv = {.k = hi->current_dr->k, .v = hi->current_dr->v};
    hi->records_to_iterate -= 1;

    return kv;
}

bool htbl_iterator_has_next(const htbl_iterator_t *hi)
{
    ASSERT_INPUT(hi);
    return hi->records_to_iterate;
}

void htbl_iterator_reset(htbl_iterator_t *hi)
{
    ASSERT_INPUT(hi);
    hi->bucket = 0;
    hi->current_dr = hi->htbl->buckets[0];
    hi->records_to_iterate = hi->htbl->number_of_records;
}

void htbl_iterator_destroy(htbl_iterator_t *hi)
{
    if (hi)
    {
        ufree(hi);
    }
}

size_t htbl_get_size(const htbl_t *h)
{
    ASSERT_INPUT(h);
    return h->number_of_records;
}

bool htbl_is_empty(const htbl_t *h)
{
    ASSERT_INPUT(h);
    return h->number_of_records == 0;
}

bool htbl_has_key(const htbl_t *h, generic_t k)
{
    ASSERT_INPUT(h);
    return *_find_by_key(h, k) != NULL;
}
