#include "bitmap.h"
#include "asserts.h"

static void _flip_range(void *b, size_t l, size_t r)
{
    while ((l < r) && (l % 8))
    {
        ubitmap_flip_bit(b, l);
        l++;
    }
    while ((l < r) && ((r - l) >= 8))
    {
        ((uint8_t *)b)[l / 8] ^= 0xff;
        l += 8;
    }
    while (l < r)
    {
        ubitmap_flip_bit(b, l);
        l++;
    }
}

void ubitmap_flip_range(void *b, size_t l, size_t r)
{
    UASSERT_INPUT(b);
    UASSERT_INPUT(l < r);
    _flip_range(b, l, r);
}

void ubitmap_flip_all(void *b, size_t len)
{
    UASSERT_INPUT(b);
    _flip_range(b, 0, len);
}

char *ubitmap_as_str(const void *b, size_t len)
{
    UASSERT_INPUT(b);
    return ubitmap_range_as_str(b, 0, len);
}

char *ubitmap_range_as_str(const void *b, size_t l, size_t r)
{
    UASSERT_INPUT(b);
    UASSERT_INPUT(l < r);

    ubuffer_t buf = {0};

    while ((l < r) && (l % 8))
    {
        ubuffer_append_byte(&buf, ubitmap_bit_is_set(b, l) + '0');
        l++;
    }

    umemchunk_t m;
    while ((l < r) && ((r - l) >= 8))
    {
        uint8_t byte = ((uint8_t *)b)[l / 8];
        char byte_str[8] = {
            !!(0x80 & byte) + '0', !!(0x40 & byte) + '0',
            !!(0x20 & byte) + '0', !!(0x10 & byte) + '0',
            !!(0x08 & byte) + '0', !!(0x04 & byte) + '0',
            !!(0x02 & byte) + '0', !!(0x01 & byte) + '0',
        };
        m.data = &byte_str;
        m.size = sizeof(byte_str);
        ubuffer_append_memchunk(&buf, &m);
        l += 8;
    }

    while (l < r)
    {
        ubuffer_append_byte(&buf, ubitmap_bit_is_set(b, l) + '0');
        l++;
    }

    ubuffer_null_terminate(&buf);

    return buf.data;
}

int ubitmap_fprint_range(const void *b, size_t l, size_t r, FILE *f)
{
    UASSERT_INPUT(b);
    UASSERT_INPUT(f);
    UASSERT_INPUT(l < r);

    char *str = ubitmap_range_as_str(b, l, r);
    int ret = fprintf(f, "%s\n", str);
    ufree(str);

    return ret;
}

int ubitmap_fprint(const void *b, size_t len, FILE *f)
{
    UASSERT_INPUT(b);
    UASSERT_INPUT(f);
    return ubitmap_fprint_range(b, 0, len, f);
}
