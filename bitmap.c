#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "generic.h"
#include "bitmap.h"

static void _flip_range(void *a, size_t l, size_t r)
{
    while ((l < r) && (l % 8))
    {
        ubitmap_flip_bit(a, l);
        l++;
    }
    while ((l < r) && ((r - l) >= 8))
    {
        ((uint8_t *)a)[l / 8] ^= 0xff;
        l += 8;
    }
    while (l < r)
    {
        ubitmap_flip_bit(a, l);
        l++;
    }
}

void ubitmap_flip_range(void *a, size_t l, size_t r)
{
    ASSERT_INPUT(a);
    ASSERT_INPUT(l < r);
    _flip_range(a, l, r);
}

void ubitmap_flip_all(void *a, size_t len)
{
    ASSERT_INPUT(a);
    _flip_range(a, 0, len);
}

char *ubitmap_as_str(const void *a, size_t len)
{
    ASSERT_INPUT(a);
    return ubitmap_range_as_str(a, 0, len);
}

char *ubitmap_range_as_str(const void *a, size_t l, size_t r)
{
    ASSERT_INPUT(a);
    ASSERT_INPUT(l < r);

    buffer_t buf = {0};

    while ((l < r) && (l % 8))
    {
        buffer_append_byte(&buf, ubitmap_get_bit(a, l) + '0');
        l++;
    }

    memchunk_t m;
    while ((l < r) && ((r - l) >= 8))
    {
        uint8_t byte = ((uint8_t *)a)[l / 8];
        char byte_str[8] = {
            !!(0x80 & byte) + '0', !!(0x40 & byte) + '0',
            !!(0x20 & byte) + '0', !!(0x10 & byte) + '0',
            !!(0x08 & byte) + '0', !!(0x04 & byte) + '0',
            !!(0x02 & byte) + '0', !!(0x01 & byte) + '0',
        };
        m.data = &byte_str;
        m.size = sizeof(byte_str);
        buffer_append_chunk(&buf, &m);
        l += 8;
    }

    while (l < r)
    {
        buffer_append_byte(&buf, ubitmap_get_bit(a, l) + '0');
        l++;
    }

    buffer_null_terminate(&buf);

    return buf.data;
}

int ubitmap_fprint_range(const void *a, size_t l, size_t r, FILE *f)
{
    ASSERT_INPUT(a);
    ASSERT_INPUT(f);
    ASSERT_INPUT(l < r);

    char *str = ubitmap_range_as_str(a, l, r);
    int ret = fprintf(f, "%s\n", str);
    ufree(str);

    return ret;
}

int ubitmap_fprint(const void *a, size_t len, FILE *f)
{
    ASSERT_INPUT(a);
    ASSERT_INPUT(f);
    return ubitmap_fprint_range(a, 0, len, f);
}
