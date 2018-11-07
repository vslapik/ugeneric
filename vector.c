#include "vector.h"

#include "asserts.h"
#include "mem.h"
#include "sort.h"

/* [0][1][2][...][size - 1][.][.][...][.][.][capacity - 1] */

struct uvector_opaq {
    uvoid_handlers_t void_handlers;
    bool is_data_owner;
    ugeneric_t *cells;
    size_t size;
    size_t capacity;
    ugeneric_sorter_t sorter;
};

static ugeneric_sorter_t _default_vector_sorter = hybrid_sort;

static uvector_t *_allocate_vector(void)
{
    uvector_t *v = umalloc(sizeof(*v));
    memset(&v->void_handlers, 0, sizeof(v->void_handlers));
    v->size = 0;
    v->capacity = 0;
    v->cells = NULL;
    v->is_data_owner = true;
    v->sorter = _default_vector_sorter;

    return v;
}

static uvector_t *_vcpy(const uvector_t *v, bool deep)
{
    UASSERT_INPUT(v);

    uvector_t *copy = _allocate_vector();
    *copy = *v;

    if (v->size)
    {
        copy->cells = umalloc(v->size * sizeof(copy->cells[0]));
    }

    copy->is_data_owner = deep;
    if (deep)
    {
        for (size_t i = 0; i < v->size; i++)
        {
            copy->cells[i] = ugeneric_copy_v(v->cells[i], v->void_handlers.cpy);
        }
    }
    else
    {
        if (v->size)
        {
            memcpy(copy->cells, v->cells, v->size * sizeof(copy->cells[0]));
        }
    }

    return copy;
}

int uvector_compare(const uvector_t *v1, const uvector_t *v2)
{
    int diff = 0;
    UASSERT_INPUT(v1);
    UASSERT_INPUT(v2);

    if (v1 == v2)
    {
        return 0;
    }

    size_t len = MIN(v1->size, v2->size);
    for (size_t i = 0; i < len; i++)
    {
        diff = ugeneric_compare_v(v1->cells[i], v2->cells[i], v1->void_handlers.cmp);
        if (diff)
        {
            break;
        }
    }

    if (diff == 0)
    {
        diff = (v1->size > v2->size) ?  1 : ((v1->size < v2->size) ? -1 : 0);
    }

    return diff;
}

uvector_t *uvector_create_with_size(size_t size, ugeneric_t value)
{
    uvector_t *v = _allocate_vector();
    if (size)
    {
        uvector_reserve_capacity(v, size);
        for (size_t i = 0; i < size; i++)
        {
            v->cells[i] = value;
        }
        v->size = size;
    }

    return v;
}

uvector_t *uvector_create(void)
{
    return _allocate_vector();
}

uvector_t *uvector_create_from_array(void *array, size_t array_len,
                                     size_t array_element_size,
                                     ugeneric_type_e uvector_element_type)
{
    UASSERT_INPUT(array);

    size_t i = 0;
    uvector_t *v = _allocate_vector();
    uvector_reserve_capacity(v, array_len);
    char *p = array;

    while (i < array_len)
    {
        v->cells[i] = (ugeneric_t){0};
        v->cells[i].t.type = uvector_element_type;
        // TODO: check portability
        memcpy(&(v->cells[i].v), p, array_element_size);
        p += array_element_size;
        i++;
    }
    v->size = array_len;

    return v;
}

void uvector_destroy(uvector_t *v)
{
    if (v)
    {
        if (v->is_data_owner)
        {
            for (size_t i = 0; i < v->size; i++)
            {
                ugeneric_destroy_v(v->cells[i], v->void_handlers.dtr);
            }
        }
        ufree(v->cells);
        ufree(v);
    }
}

ugeneric_t uvector_get_at(const uvector_t *v, size_t i)
{
    UASSERT_INPUT(v);
    UASSERT_INPUT(i < v->size);
    return v->cells[i];
}

ugeneric_t uvector_get_at_random(const uvector_t *v)
{
    UASSERT_INPUT(v);
    return v->cells[ugeneric_random_from_range(0, v->size - 1)];
}

void uvector_set_at(uvector_t *v, size_t i, ugeneric_t e)
{
    UASSERT_INPUT(v);
    UASSERT_INPUT(i < v->size);

    if (v->is_data_owner)
    {
        ugeneric_destroy_v(v->cells[i], v->void_handlers.dtr);
    }
    v->cells[i] = e;
}

ugeneric_t *uvector_get_cells(const uvector_t *v)
{
    UASSERT_INPUT(v);
    return v->cells;
}

void uvector_swap(uvector_t *v, size_t l, size_t r)
{
    UASSERT_INPUT(v);
    UASSERT_INPUT(l < v->size);
    UASSERT_INPUT(r < v->size);

    ugeneric_swap(&v->cells[l], &v->cells[r]);
}

void uvector_resize(uvector_t *v, size_t new_size, ugeneric_t value)
{
    UASSERT_INPUT(v);

    uvector_reserve_capacity(v, new_size);
    for (size_t i = v->size; i < new_size; i++)
    {
        v->cells[i] = value;
    }
    if (v->is_data_owner)
    {
        for (size_t i = new_size; i < v->size; i++)
        {
            ugeneric_destroy_v(v->cells[i], v->void_handlers.dtr);
        }
    }
    v->size = new_size;
}

void uvector_shrink_to_size(uvector_t *v)
{
    UASSERT_INPUT(v);

    if (v->capacity && v->size && (v->capacity > v->size))
    {
        void *p = urealloc(v->cells, v->size * sizeof(v->cells[0]));
        v->cells = p;
        v->capacity = v->size;
    }
}

void uvector_reserve_capacity(uvector_t *v, size_t new_capacity)
{
    UASSERT_INPUT(v);

    if (v->capacity < new_capacity)
    {
        void *p = urealloc(v->cells, new_capacity * sizeof(v->cells[0]));
        v->cells = p;
        v->capacity = new_capacity;
    }
}

size_t uvector_get_capacity(const uvector_t *v)
{
    UASSERT_INPUT(v);
    return v->capacity;
}

size_t uvector_get_size(const uvector_t *v)
{
    UASSERT_INPUT(v);
    return v->size;
}

bool uvector_is_empty(const uvector_t *v)
{
    UASSERT_INPUT(v);
    return v->size == 0;
}

void uvector_insert_at(uvector_t *v, size_t i, ugeneric_t e)
{
    UASSERT_INPUT(v);
    UASSERT_INPUT(i < v->size);

    uvector_reserve_capacity(v, v->size + 1);
    memmove(v->cells + i + 1, v->cells + i, (v->size - i) * sizeof(v->cells[0]));
    v->cells[i] = e;
    v->size++;
}

void uvector_remove_at(uvector_t *v, size_t i)
{
    UASSERT_INPUT(v);
    UASSERT_INPUT(i < v->size);

    if (v->is_data_owner)
    {
        ugeneric_destroy_v(v->cells[i], v->void_handlers.dtr);
    }
    memmove(v->cells + i, v->cells + i + 1, (v->size - i - 1) * sizeof(v->cells[0]));
    v->size--;
}

void uvector_clear(uvector_t *v)
{
    UASSERT_INPUT(v);

    if (v->size)
    {
        if (v->is_data_owner)
        {
            for (size_t i = 0; i < v->size; i++)
            {
                ugeneric_destroy_v(v->cells[i], v->void_handlers.dtr);
            }
        }
    }
    v->size = 0;
}

void uvector_append(uvector_t *v, ugeneric_t e)
{
    UASSERT_INPUT(v);

    if (v->capacity == v->size)
    {
        size_t new_capacity = MAX(SCALE_FACTOR * v->size,
                                  VECTOR_INITIAL_CAPACITY);
        uvector_reserve_capacity(v, new_capacity ? new_capacity : 1);
    }
    v->cells[v->size++] = e;
}

ugeneric_t uvector_get_back(const uvector_t *v)
{
    UASSERT_INPUT(v);
    return v->cells[v->size - 1];
}

ugeneric_t uvector_pop_at(uvector_t *v, size_t i)
{
    UASSERT_INPUT(v);
    UASSERT_INPUT(i < v->size);

    ugeneric_t e = v->cells[i];
    memmove(v->cells + i, v->cells - i + 1, (v->size - i - 1) * sizeof(v->cells[0]));
    v->size--;

    return e;
}

ugeneric_t uvector_pop_back(uvector_t *v)
{
    UASSERT_INPUT(v);
    UASSERT_INPUT(v->size);
    return v->cells[--v->size];
}

void uvector_reverse(uvector_t *v)
{
    ugeneric_array_reverse(v->cells, v->size, 0, v->size ? v->size -1 : 0);
}

void uvector_reverse_range(uvector_t *v, size_t l, size_t r)
{
    ugeneric_array_reverse(v->cells, v->size, l, r);
}

void uvector_sort(uvector_t *v)
{
    UASSERT_INPUT(v);
    v->sorter(v->cells, v->size, v->void_handlers.cmp);
}

bool uvector_is_sorted(const uvector_t *v)
{
    return ugeneric_array_is_sorted(v->cells, v->size, v->void_handlers.cmp);
}

uvector_t *uvector_copy(const uvector_t *v)
{
    return _vcpy(v, false);
}

uvector_t *uvector_deep_copy(const uvector_t *v)
{
    return _vcpy(v, true);
}

void uvector_serialize(const uvector_t *v, ubuffer_t *buf)
{
    UASSERT_INPUT(v);
    UASSERT_INPUT(buf);

    ubuffer_append_byte(buf, '[');
    for (size_t i = 0; i < v->size; i++)
    {
        ugeneric_serialize_v(v->cells[i], buf, v->void_handlers.s8r);
        if (i < v->size - 1)
        {
            ubuffer_append_data(buf, ", ", 2);
        }
    }
    ubuffer_append_byte(buf, ']');
}

char *uvector_as_str(const uvector_t *v)
{
    UASSERT_INPUT(v);

    ubuffer_t buf = {0};
    uvector_serialize(v, &buf);
    ubuffer_null_terminate(&buf);

    return buf.data;
}

int uvector_fprint(const uvector_t *v, FILE *out)
{
    UASSERT_INPUT(v);
    UASSERT_INPUT(out);

    char *str = uvector_as_str(v);
    int ret = fprintf(out, "%s\n", str);
    ufree(str);

    return ret;
}

size_t uvector_bsearch(const uvector_t *v, ugeneric_t e)
{
    UASSERT_INPUT(v);
    return ugeneric_array_bsearch(v->cells, v->size, e, v->void_handlers.cmp);
}

bool uvector_next_permutation(uvector_t *v)
{
    UASSERT_INPUT(v);
    return ugeneric_array_next_permutation(v->cells, v->size, v->void_handlers.cmp);
}

ugeneric_base_t *uvector_get_base(uvector_t *v)
{
    UASSERT_INPUT(v);
    return (ugeneric_base_t *)v;
}

uvector_t *uvector_get_slice(const uvector_t *v, size_t begin, size_t end,
                             size_t stride)
{
    UASSERT_INPUT(v);
    UASSERT_INPUT(begin <= end);
    UASSERT_INPUT(end <= v->size);
    UASSERT_INPUT(stride != 0);

    uvector_t *slice = umalloc(sizeof(*v));
    memcpy(&slice->void_handlers, &v->void_handlers, sizeof(v->void_handlers));
    slice->size = (end - begin) / stride + (bool)((end - begin) % stride);
    slice->capacity = slice->size;
    slice->is_data_owner = false;
    slice->sorter = _default_vector_sorter;
    slice->cells = NULL;
    if (slice->size)
    {
        slice->cells = umalloc(sizeof(slice->cells[0]) * slice->size);
        for (size_t i = 0; i < slice->size; i++)
        {
            slice->cells[i] = v->cells[begin + i * stride];
        }
    }

    return slice;
}

ugeneric_t *uvector_find(uvector_t *v, ugeneric_t e)
{
    UASSERT_INPUT(v);

    ugeneric_t *ret = NULL;
    for (size_t i = 0; i < v->size; i++)
    {
        if (ugeneric_compare_v(v->cells[i], e, v->void_handlers.cmp) == 0)
        {
            ret = &v->cells[i];
            break;
        }
    }

    return ret;
}

bool uvector_contains(const uvector_t *v, ugeneric_t e)
{
    return uvector_find((uvector_t *)v, e) != NULL;
}

void uvector_dump_to_gnuplot(const uvector_t *v, gnuplot_attrs_t *attrs, FILE *out)
{
    fprintf(out,
        "set datafile separator ','\n"
        //"set terminal png size 1024,768\n"
        "set title '%s'\n"
        "set xlabel '%s'\n"
        "set ylabel '%s'\n"
//        "set xdata time\n"
//        "set timefmt \"%s\""
//        "set format x \"%m/%d\""
        "set key left top\n"
        "set grid\n",
        attrs->title, attrs->xlabel, attrs->ylabel
    );
    fprintf(out, "plot '-' with lines title '%s'\n", attrs->data_label);
    for (size_t i = 0; i < v->size; i++)
    {
        char *str = ugeneric_as_str(v->cells[i]);
        fprintf(out, "%zu,%s\n", i, str);
        ufree(str);
    }
}
