#include "set.h"
#include "asserts.h"

uset_t *uset_create(void)
{
    uset_t *s = umalloc(sizeof(*s));
    s->data = udict_create();
    return s;
}

void uset_destroy(uset_t *s)
{
    if (s)
    {
        udict_destroy(s->data);
        ufree(s);
    }
}

void uset_put(uset_t *s, ugeneric_t e)
{
    UASSERT_INPUT(s);

    ugeneric_t g = udict_get(s->data, e, G_SIZE(0));
    UASSERT(G_AS_SIZE(g) < SIZE_MAX);
    g = G_SIZE(G_AS_SIZE(g) + 1);
    udict_put(s->data, e, g);
}

size_t uset_has_element(const uset_t *s, ugeneric_t e)
{
    UASSERT_INPUT(s);

    ugeneric_t g = udict_get(s->data, e, G_ERROR(""));
    if (G_IS_ERROR(g))
    {
        return 0;
    }

    UASSERT_INTERNAL(G_IS_SIZE(g));

    return G_AS_SIZE(g);
}

void uset_serialize(const uset_t *s, ubuffer_t *buf)
{
    UASSERT_INPUT(s);
    UASSERT_INPUT(buf);

    udict_iterator_t *di = udict_iterator_create(s->data);
    void_s8r_t s8r = udict_get_void_serializer(s->data);

    ubuffer_append_byte(buf, '[');
    while (udict_iterator_has_next(di))
    {
        ugeneric_kv_t kv = udict_iterator_get_next(di);
        ugeneric_serialize_v(kv.k, buf, s8r);
        if (udict_iterator_has_next(di))
        {
            ubuffer_append_data(buf, ", ", 2);
        }
    }
    udict_iterator_destroy(di);
    ubuffer_append_byte(buf, ']');
}

char *uset_as_str(const uset_t *s)
{
    UASSERT_INPUT(s);

    ubuffer_t buf = {0};
    uset_serialize(s, &buf);
    ubuffer_null_terminate(&buf);

    return buf.data;
}

int uset_fprint(const uset_t *s, FILE *out)
{
    UASSERT_INPUT(s);
    UASSERT_INPUT(out);

    char *str = uset_as_str(s);
    int ret = fprintf(out, "%s\n", str);
    ufree(str);

    return ret;
}

uvector_t *uset_get_elements(const uset_t *s, bool deep)
{
    UASSERT_INPUT(s);
    return udict_get_items(s->data, UDICT_KEYS, deep);
}

void uset_update_from_vector(uset_t *s, uvector_t *v, bool deep)
{
    size_t size = uvector_get_size(v);
    for (size_t i = 0; i < size; i++)
    {
        ugeneric_t g = uvector_get_at(v, i);
        uset_put(s, deep ? ugeneric_copy(g, uvector_get_void_copier(v)) : g);
    }
}
