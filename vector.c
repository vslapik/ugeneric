#include <stdlib.h>
#include <string.h>
#include "vector.h"
#include "mem.h"
#include "sort.h"

/* [0][1][2][...][size - 1][.][.][...][.][.][capacity - 1] */

struct vector_opaq {
    generic_t *cells;
    size_t size;
    size_t capacity;
    bool is_data_owner;
    void_cpy_t cpy;
    void_cmp_t cmp;
    void_dtr_t dtr;
};

static vector_t *_vcpy(const vector_t *v, bool deep)
{
    vector_t *copy = vector_create();
    memcpy(copy, v, sizeof(*v));
    copy->cells = umalloc(v->size * sizeof(copy->cells[0]));

    if (deep)
    {
        copy->is_data_owner = true;
        for (size_t i = 0; i < v->size; i++)
        {
            copy->cells[i] = generic_copy(v->cells[i], v->cpy);
        }
    }
    else
    {
        copy->is_data_owner = false;
        copy->cpy = 0;
        copy->cmp = 0;
        copy->dtr = 0;
        memcpy(copy->cells, v->cells, v->size * sizeof(copy->cells[0]));
    }

    return copy;
}

static vector_t *_allocate_vector(void)
{
    vector_t *v = umalloc(sizeof(*v));
    v->size = 0;
    v->capacity = 0;
    v->cells = NULL;
    v->is_data_owner = true;
    v->cpy = NULL;
    v->cmp = NULL;
    v->dtr = NULL;

    return v;
}

int vector_compare(const vector_t *v1, const vector_t *v2, void_cmp_t cmp)
{
    ASSERT_INPUT(v1);
    ASSERT_INPUT(v2);
    ASSERT_INPUT(v1->size == v2->size);

    int diff = 0;
    for (size_t i = 0; i < v1->size; i++)
    {
        diff = generic_compare(v1->cells[i], v2->cells[i], cmp);
        if (diff)
        {
            break;
        }
    }

    return diff;
}

vector_t *vector_create_with_size(size_t size, generic_t value)
{
    vector_t *v = _allocate_vector();
    if (size)
    {
        vector_reserve_capacity(v, size);
        for (size_t i = 0; i < size; i++)
        {
            v->cells[i] = value;
        }
        v->size = size;
    }

    return v;
}

vector_t *vector_create(void)
{
    return _allocate_vector();
}

vector_t *vector_create_from_array(void *array, size_t array_len,
                                   size_t array_element_size,
                                   generic_type_e vector_element_type)
{
    size_t i = 0;
    vector_t *v = _allocate_vector();
    vector_reserve_capacity(v, array_len);
    char *p = array;

    while (i < array_len)
    {
        v->cells[i] = (generic_t){0};
        v->cells[i].type.type = vector_element_type;
        // TODO: check portability
        memcpy(&(v->cells[i].value), p, array_element_size);
        p += array_element_size;
        i++;
    }
    v->size = array_len;

    return v;
}

void vector_take_data_ownership(vector_t *v)
{
    ASSERT_INPUT(v);
    v->is_data_owner = true;
}

void vector_drop_data_ownership(vector_t *v)
{
    ASSERT_INPUT(v);
    v->is_data_owner = false;
}

void vector_set_destroyer(vector_t *v, void_dtr_t dtr)
{
    ASSERT_INPUT(v);
    v->dtr = dtr;
}

void vector_set_comparator(vector_t *v, void_cmp_t cmp)
{
    ASSERT_INPUT(v);
    v->cmp = cmp;
}

void vector_set_copier(vector_t *v, void_cpy_t cpy)
{
    ASSERT_INPUT(v);
    v->cpy = cpy;
}

void_dtr_t vector_get_destroyer(const vector_t *v)
{
    ASSERT_INPUT(v);
    return v->dtr;
}

void_cmp_t vector_get_comparator(const vector_t *v)
{
    ASSERT_INPUT(v);
    return v->cmp;
}

void_cpy_t vector_get_copier(const vector_t *v)
{
    ASSERT_INPUT(v);
    return v->cpy;
}

void vector_destroy(vector_t *v)
{
    if (v)
    {
        if (v->is_data_owner)
        {
            for (size_t i = 0; i < v->size; i++)
            {
                generic_destroy(v->cells[i], v->dtr);
            }
        }
        ufree(v->cells);
        ufree(v);
    }
}

generic_t vector_get_at(const vector_t *v, size_t i)
{
    ASSERT_INPUT(v);
    ASSERT_INPUT(i < v->size);
    return v->cells[i];
}

generic_t vector_get_at_random(const vector_t *v)
{
    ASSERT_INPUT(v);
    return v->cells[random_from_range(0, v->size - 1)];
}

void vector_set_at(vector_t *v, size_t i, generic_t e)
{
    ASSERT_INPUT(v);
    ASSERT_INPUT(i < v->size);

    if (v->is_data_owner)
    {
        generic_destroy(v->cells[i], v->dtr);
    }
    v->cells[i] = e;
}

generic_t *vector_get_cells(const vector_t *v)
{
    ASSERT_INPUT(v);
    return v->cells;
}

void vector_swap(vector_t *v, size_t l, size_t r)
{
    ASSERT_INPUT(v);
    ASSERT_INPUT(l < v->size);
    ASSERT_INPUT(r < v->size);

    generic_swap(&v->cells[l], &v->cells[r]);
}

void vector_resize(vector_t *v, size_t new_size, generic_t value)
{
    ASSERT_INPUT(v);

    vector_reserve_capacity(v, new_size);
    for (size_t i = v->size; i < new_size; i++)
    {
        v->cells[i] = value;
    }
    if (v->is_data_owner)
    {
        for (size_t i = new_size; i < v->size; i++)
        {
            generic_destroy(v->cells[i], v->dtr);
        }
    }
    v->size = new_size;
}

void vector_shrink_to_size(vector_t *v)
{
    ASSERT_INPUT(v);

    if (v->capacity && v->size)
    {
        void *p = urealloc(v->cells, v->size * sizeof(v->cells[0]));
        v->cells = p;
        v->capacity = v->size;
    }
}

void vector_reserve_capacity(vector_t *v, size_t new_capacity)
{
    ASSERT_INPUT(v);

    if (v->capacity < new_capacity)
    {
        void *p = urealloc(v->cells, new_capacity * sizeof(v->cells[0]));
        v->cells = p;
        v->capacity = new_capacity;
    }
}

size_t vector_get_capacity(const vector_t *v)
{
    ASSERT_INPUT(v);
    return v->capacity;
}

size_t vector_get_size(const vector_t *v)
{
    ASSERT_INPUT(v);
    return v->size;
}

bool vector_is_empty(const vector_t *v)
{
    ASSERT_INPUT(v);
    return v->size == 0;
}

void vector_insert_at(vector_t *v, size_t i, generic_t e)
{
    ASSERT_INPUT(v);
    ASSERT_INPUT(i < v->size);

    vector_reserve_capacity(v, v->size + 1);
    memmove(v->cells + i + 1, v->cells + i, (v->size - i) * sizeof(v->cells[0]));
    v->cells[i] = e;
    v->size++;
}

void vector_remove_at(vector_t *v, size_t i)
{
    ASSERT_INPUT(v);
    ASSERT_INPUT(i < v->size);

    if (v->is_data_owner)
    {
        generic_destroy(v->cells[i], v->dtr);
    }
    memmove(v->cells + i, v->cells + i + 1, (v->size - i - 1) * sizeof(v->cells[0]));
    v->size--;
}

void vector_clear(vector_t *v)
{
    ASSERT_INPUT(v);

    if (v->size)
    {
        if (v->is_data_owner)
        {
            for (size_t i = 0; i < v->size; i++)
            {
                generic_destroy(v->cells[i], v->dtr);
            }
        }
        ufree(v->cells);
        v->cells = 0; // if you don't do it realloc(v->cells, ...) will fail
    }
    v->size = 0;
    v->capacity = 0;
}

void vector_append(vector_t *v, generic_t e)
{
    ASSERT_INPUT(v);

    if (v->capacity == v->size)
    {
        size_t new_capacity = MAX(SCALE_FACTOR * v->size,
                                  VECTOR_INITIAL_CAPACITY);
        vector_reserve_capacity(v, new_capacity ? new_capacity : 1);
    }
    v->cells[v->size++] = e;
}

generic_t vector_get_back(const vector_t *v)
{
    ASSERT_INPUT(v);
    return v->cells[v->size - 1];
}

generic_t vector_pop_at(vector_t *v, size_t i)
{
    ASSERT_INPUT(v);
    ASSERT_INPUT(i < v->size);

    generic_t e = v->cells[i];
    memmove(v->cells + i, v->cells - i + 1, (v->size - i - 1) * sizeof(v->cells[0]));
    v->size--;

    return e;
}

generic_t vector_pop_back(vector_t *v)
{
    ASSERT_INPUT(v);
    ASSERT_INPUT(v->size);
    return v->cells[--v->size];
}

void vector_reverse(vector_t *v, size_t l, size_t r)
{
    generic_array_reverse(v->cells, v->size, l, r);
}

void vector_sort(vector_t *v)
{
    ASSERT_INPUT(v);
    quick_sort(v->cells, v->size, v->cmp);
}

vector_t *vector_copy(const vector_t *v)
{
    ASSERT_INPUT(v);
    return _vcpy(v, false);
}

vector_t *vector_deep_copy(const vector_t *v)
{
    ASSERT_INPUT(v);
    return _vcpy(v, true);
}

void vector_serialize(const vector_t *v, buffer_t *buf)
{
    ASSERT_INPUT(v);
    ASSERT_INPUT(buf);

    buffer_append_byte(buf, '[');
    for (size_t i = 0; i < v->size; i++)
    {
        generic_serialize(v->cells[i], buf);
        if (i < v->size - 1)
        {
            buffer_append_data(buf, ", ", 2);
        }
    }
    buffer_append_byte(buf, ']');
}

char *vector_as_str(const vector_t *v)
{
    ASSERT_INPUT(v);

    buffer_t buf = {0};
    vector_serialize(v, &buf);
    buffer_null_terminate(&buf);

    return buf.data;
}

int vector_print(const vector_t *v)
{
    ASSERT_INPUT(v);
    return vector_fprint(v, stdout);
}

int vector_fprint(const vector_t *v, FILE *out)
{
    ASSERT_INPUT(v);
    ASSERT_INPUT(out);

    char *str = vector_as_str(v);
    int ret = fprintf(out, "%s\n", str);
    ufree(str);

    return ret;
}

size_t vector_bsearch(const vector_t *v, generic_t e)
{
    ASSERT_INPUT(v);
    return generic_bsearch(v->cells, v->size, e, v->cmp);
}

bool vector_next_permutation(vector_t *v)
{
    ASSERT_INPUT(v);
    return generic_next_permutation(v->cells, v->size, v->cmp);
}
