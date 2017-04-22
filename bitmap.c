#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "generic.h"
#include "mem.h"
#include "bitmap.h"

struct bitmap_opaq {
    uint8_t *data;
    size_t size;     // size is in bits
    size_t capacity; // capacity is in bytes
};

static void _flip_range(bitmap_t *b, size_t l, size_t r)
{
    while ((l <= r) && (l % 8))
    {
        FLIP_BIT(b->data, l);
        l++;
    }
    while ((l <= r) && ((r - l) >= 8))
    {
        b->data[l / 8] ^= 0xff;
        l += 8;
    }
    while (l <= r)
    {
        FLIP_BIT(b->data, l);
        l++;
    }
}

bitmap_t *bitmap_create(size_t size)
{
    ASSERT_INPUT(size);

    bitmap_t *b = umalloc(sizeof(bitmap_t));
    b->capacity = size / 8 + !!(size % 8);
    b->data = ucalloc(b->capacity, 1);
    b->size = size;

    return b;
}

void bitmap_destroy(bitmap_t *b)
{
    if (b)
    {
        ufree(b->data);
        ufree(b);
    }
}

size_t bitmap_get_size(const bitmap_t *b)
{
    ASSERT_INPUT(b);
    return b->size;
}

void bitmap_set_bit(bitmap_t *b, size_t i)
{
    ASSERT_INPUT(b);
    ASSERT_INPUT(i < b->size);
    SET_BIT(b->data, i);
}

bool bitmap_get_bit(const bitmap_t *b, size_t i)
{
    ASSERT_INPUT(b);
    ASSERT_INPUT(i < b->size);
    return GET_BIT(b->data, i);
}

void bitmap_clear_bit(bitmap_t *b, size_t i)
{
    ASSERT_INPUT(b);
    ASSERT_INPUT(i < b->size);
    CLR_BIT(b->data, i);
}

void bitmap_flip_range(bitmap_t *b, size_t l, size_t r)
{
    ASSERT_INPUT(b);
    _flip_range(b, l, r);
}

void bitmap_flip_all(bitmap_t *b)
{
    ASSERT_INPUT(b);
    _flip_range(b, 0, b->size - 1);
}

char *bitmap_as_str(const bitmap_t *b)
{
    ASSERT_INPUT(b);
    return bitmap_range_as_str(b, 0, b->size - 1);
}

char *bitmap_range_as_str(const bitmap_t *b, size_t l, size_t r)
{
    ASSERT_INPUT(b);
    ASSERT_INPUT(l <= r);

    buffer_t buf = {0};

    while ((l <= r) && (l % 8))
    {
        buffer_append_byte(&buf, GET_BIT(b->data, l) + '0');
        l++;
    }

    memchunk_t m;
    while ((l <= r) && ((r - l) >= 8))
    {
        uint8_t byte = b->data[l / 8];
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

    while (l <= r)
    {
        buffer_append_byte(&buf, GET_BIT(b->data, l) + '0');
        l++;
    }

    buffer_null_terminate(&buf);

    return buf.data;
}

int bitmap_fprint_range(const bitmap_t *b, size_t l, size_t r, FILE *f)
{
    ASSERT_INPUT(b);
    ASSERT_INPUT(f);
    ASSERT_INPUT(l <= r);

    char *str = bitmap_range_as_str(b, l, r);
    int ret = fprintf(f, "%s\n", str);
    ufree(str);

    return ret;
}

int bitmap_fprint(const bitmap_t *b, FILE *f)
{
    ASSERT_INPUT(b);
    ASSERT_INPUT(f);
    return bitmap_fprint_range(b, 0, b->size - 1, f);
}
