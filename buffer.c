#include <string.h>
#include <stdint.h>
#include "mem.h"
#include "buffer.h"
#include "asserts.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

static void _reserve_capacity(buffer_t *buf, size_t new_capacity)
{
    if (buf->capacity < new_capacity)
    {
        new_capacity = MAX(new_capacity * BUFFER_SCALE_FACTOR,
                              BUFFER_INITIAL_CAPACITY);
        void *p = urealloc(buf->data, new_capacity);
        buf->data = p;
        buf->capacity = new_capacity;
    }
}

void buffer_append_data(buffer_t *buf, const void *data, size_t size)
{
    ASSERT_INPUT(buf);
    ASSERT_INPUT(data);

    _reserve_capacity(buf, buf->data_size + size);
    memcpy((char *)buf->data + buf->data_size, data, size);
    buf->data_size += size;
}

void buffer_append_chunk(buffer_t *buf, const memchunk_t *chunk)
{
    ASSERT_INPUT(buf);
    ASSERT_INPUT(chunk);

    _reserve_capacity(buf, buf->data_size + chunk->size);
    memcpy((char *)buf->data + buf->data_size, chunk->data, chunk->size);
    buf->data_size += chunk->size;
}

void buffer_append_byte(buffer_t *buf, char byte)
{
    ASSERT_INPUT(buf);

    _reserve_capacity(buf, buf->data_size + 1);
    ((char *)buf->data)[buf->data_size] = byte;
    buf->data_size += 1;
}

void buffer_append_string(buffer_t *buf, const char *str)
{
    ASSERT_INPUT(buf);

    size_t slen = strlen(str);
    _reserve_capacity(buf, buf->data_size + slen);
    memcpy((char *)buf->data + buf->data_size, str, slen);
    buf->data_size += slen;
}

void buffer_null_terminate(buffer_t *buf)
{
    ASSERT_INPUT(buf);

    if (buf->data_size && (((char *)buf->data)[buf->data_size - 1] != '\0'))
    {
        buffer_append_byte(buf, '\0');
    }
}

void buffer_reset(buffer_t *buf)
{
    ASSERT_INPUT(buf);

    buf->data_size = 0;
}

int memchunk_fprint(memchunk_t m, FILE *out)
{
    ASSERT_INPUT(out);

    char *str = memchunk_as_str(m);
    int ret = fprintf(out, "%s\n", str);
    ufree(str);

    return ret;
}

int memchunk_print(memchunk_t m)
{
    return memchunk_fprint(m, stdout);
}

void memchunk_serialize(memchunk_t m, buffer_t *buf)
{
    ASSERT_INPUT(buf);
    ASSERT_INPUT(m.size < SIZE_MAX / 2);

    const char *hex = "0123456789abcdef";

    _reserve_capacity(buf, 2 * m.size);
    for (size_t i = 0; i < m.size; i++)
    {
        unsigned char d = ((unsigned char *)m.data)[i];
        ((char *)buf->data)[buf->data_size + 2 * i] = hex[d / 16];
        ((char *)buf->data)[buf->data_size + 2 * i + 1] = hex[d % 16];
    }
    buf->data_size += (2 * m.size);
}

char *memchunk_as_str(memchunk_t m)
{
    buffer_t buf = {0};
    memchunk_serialize(m, &buf);
    buffer_null_terminate(&buf);

    return buf.data;
}
