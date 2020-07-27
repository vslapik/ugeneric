#include "struct.h"

#include "asserts.h"
#include "string_utils.h"

struct _array_placeholder {
    size_t len;
    void *cells;
};
static const size_t _array_cells_offset = offsetof(struct _array_placeholder, cells);

ugeneric_t ustruct_create_from_dict(const udict_t *d, size_t struct_size,
                                    const ustruct_data_descriptor_t *sdd)
{
    UASSERT_INPUT(d);
    UASSERT_INPUT(sdd);

    const ustruct_data_descriptor_t *i = sdd;
    void *p = uzalloc(struct_size);

    while (i->name)
    {
        ugeneric_t g = udict_get(d, G_CSTR(i->name), i->dflt);
        if (G_IS_ERROR(g))
        {
            UABORT(ustring_fmt("default value for field '%s' is not provided", i->name));
        }
        if (i->is_array)
        {
            if (ugeneric_get_type(g) != G_VECTOR_T)
            {
                // Parsed data type doesn't match to what is expected,
                // fall back to default value.
                g = i->dflt;
            }
        }
        else
        {
            if (i->type != ugeneric_get_type(g))
            {
                // Parsed data type doesn't match to what is expected,
                // fall back to default value.
                g = i->dflt;
            }
        }
        switch (i->type)
        {
            case G_STR_T:
                if (i->is_array)
                {
                    uvector_t *v = G_AS_PTR(g);
                    char **q = NULL;
                    size_t len = uvector_get_size(v);
                    if (len)
                    {
                        q = umalloc(len * sizeof(*q));
                        ugeneric_t *cells = uvector_get_cells(v);
                        for (size_t j = 0; j < len; j++)
                        {
                            ugeneric_t e = cells[j];
                            if (ugeneric_get_type(e) != i->type)
                            {
                                // TBD: what to do if elements of vector have unexpected type
                                ufree(q);
                                goto format_error;
                            }
                            q[j] = ustring_dup(G_AS_STR(e));
                        }
                    }
                    *(size_t *)((char *)p + i->offset) = len;
                    *(char ***)((char *)p + i->offset + _array_cells_offset) = q;
                }
                else
                {
                    *(char **)((char *)p + i->offset) = ustring_dup(G_AS_STR(g));
                }
                break;
            case G_INT_T:
                if (i->is_array)
                {
                    uvector_t *v = G_AS_PTR(g);
                    long int *q = NULL;
                    size_t len = uvector_get_size(v);
                    if (len)
                    {
                        q = umalloc(len * sizeof(*q));
                        ugeneric_t *cells = uvector_get_cells(v);
                        for (size_t j = 0; j < len; j++)
                        {
                            ugeneric_t e = cells[j];
                            if (ugeneric_get_type(e) != i->type)
                            {
                                ufree(q);
                                goto format_error;
                            }
                            q[j] = G_AS_INT(e);
                        }
                    }
                    *(size_t *)((char *)p + i->offset) = len;
                    *(long int **)((char *)p + i->offset + _array_cells_offset) = q;
                }
                else
                {
                    *(long int *)((char *)p + i->offset) = G_AS_INT(g);
                }
                break;
            case G_BOOL_T:
                if (i->is_array)
                {
                    uvector_t *v = G_AS_PTR(g);
                    bool *q = NULL;
                    size_t len = uvector_get_size(v);
                    if (len)
                    {
                        q = umalloc(len * sizeof(*q));
                        ugeneric_t *cells = uvector_get_cells(v);
                        for (size_t j = 0; j < len; j++)
                        {
                            ugeneric_t e = cells[j];
                            if (ugeneric_get_type(e) != i->type)
                            {
                                ufree(q);
                                goto format_error;
                            }
                            q[j] = G_AS_BOOL(e);
                        }
                    }
                    *(size_t *)((char *)p + i->offset) = len;
                    *(bool **)((char *)p + i->offset + _array_cells_offset) = q;
                }
                else
                {
                    *(bool *)((char *)p + i->offset) = G_AS_BOOL(g);
                }
                break;
            case G_DICT_T:
                if (i->is_array)
                {
                    uvector_t *v = G_AS_PTR(g);
                    void **q = NULL;
                    size_t len = uvector_get_size(v);
                    if (len)
                    {
                        q = umalloc(len * sizeof(*q));
                        ugeneric_t *cells = uvector_get_cells(v);
                        for (size_t j = 0; j < len; j++)
                        {
                            ugeneric_t e = cells[j];
                            if (ugeneric_get_type(e) != i->type)
                            {
                                ufree(q);
                                goto format_error;
                            }
                            ugeneric_t t = ustruct_create_from_dict(G_AS_PTR(e), i->field_size, i->field_descriptor);
                            if (G_IS_ERROR(t))
                            {
                                ufree(p);
                                ufree(q);
                                return t;
                            }
                            q[j] = G_AS_PTR(t);
                        }
                    }
                    *(size_t *)((char *)p + i->offset) = len;
                    *(void ***)((char *)p + i->offset + _array_cells_offset) = q;
                }
                else
                {
                    ugeneric_t t = ustruct_create_from_dict(G_AS_PTR(g), i->field_size, i->field_descriptor);
                    if (G_IS_ERROR(t))
                    {
                        ufree(p);
                        return t;
                    }
                    *(void **)((char *)p + i->offset) = G_AS_PTR(t);
                }
                break;
            default:
                UABORT("data descriptor format error");

        }
        i++;
    }

    return G_PTR(p);

format_error:
    ufree(p);
    return G_ERROR(ustring_fmt("input data format error"));
}

void ustruct_destroy_by_descriptor(void *p,
                                   const ustruct_data_descriptor_t *sdd)
{
    UASSERT_INPUT(p);
    UASSERT_INPUT(sdd);

    const ustruct_data_descriptor_t *i = sdd;

    while (i->name)
    {
        switch (i->type)
        {
            case G_STR_T:
                if (i->is_array)
                {
                    size_t len = *(size_t *)((char *)p + i->offset);
                    char **array = *(char ***)((char *)p + i->offset + _array_cells_offset);
                    if (len)
                    {
                        for (size_t j = 0; j < len; j++)
                        {
                            ufree(array[j]);
                        }
                        ufree(array);
                    }
                }
                else
                {
                    char *str = *(char **)((char *)p + i->offset);
                    ufree(str);
                }
                break;
            case G_INT_T:
                if (i->is_array)
                {
                    size_t len = *(long int *)((char *)p + i->offset);
                    long int *array = *(long int **)((char *)p + i->offset + _array_cells_offset);
                    if (len)
                    {
                        ufree(array);
                    }
                }
                break;
            case G_BOOL_T:
                if (i->is_array)
                {
                    size_t len = *(size_t *)((char *)p + i->offset);
                    bool *array = *(bool **)((char *)p + i->offset + _array_cells_offset);
                    if (len)
                    {
                        ufree(array);
                    }
                }
                break;
            case G_DICT_T:
                if (i->is_array)
                {
                    size_t len = *(size_t *)((char *)p + i->offset);
                    void **array = *(void ***)((char *)p + i->offset + _array_cells_offset);
                    if (len)
                    {
                        for (size_t j = 0; j < len; j++)
                        {
                            ustruct_destroy_by_descriptor(array[j], i->field_descriptor);
                        }
                        ufree(array);
                    }
                }
                else
                {
                    void *q = *(void **)((char *)p + i->offset);
                    ustruct_destroy_by_descriptor(q, i->field_descriptor);
                }
                break;
            default:
                UABORT("data descriptor format error");
        }
        i++;
    }

    ufree(p);
}
