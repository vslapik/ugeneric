#include <stdio.h>
#include "generic.h"
#include "mem.h"

static bool _default_oom_handler(void *ctx)
{
    (void)ctx;
    fprintf(stderr, "seems we're gonna to be screwed ...\n");
    return false;
}

static oom_handler_t _oom_handler = _default_oom_handler;
static void *_oom_data = NULL;

void libugeneric_set_oom_handler(oom_handler_t handler, void *ctx)
{
    _oom_handler = handler;
    _oom_data = ctx;
}

void *umalloc(size_t size)
{
    /*
     * Allocating zero bytes summons implementation
     * defined behavior, better to bail out with assert.
     */
    UASSERT_INPUT(size);

    void *p = malloc(size);

    if (!p)
    {
        if (_oom_handler(_oom_data))
        {
            p = malloc(size);
        }
    }

    if (!p)
    {
        fprintf(stderr, "out of memory error\n");
        utrace_print();
        exit(UGENERIC_EXIT_OOM);
    }

    return p;
}

void *ucalloc(size_t nmemb, size_t size)
{
    /*
     * Allocating zero bytes summons implementation
     * defined behavior, better to bail out with assert.
     */
    UASSERT_INPUT(nmemb && size);

    void *p = calloc(nmemb, size);

    if (!p)
    {
        if (_oom_handler(_oom_data))
        {
            p = calloc(nmemb, size);
        }
    }

    if (!p)
    {
        fprintf(stderr, "out of memory error\n");
        utrace_print();
        exit(UGENERIC_EXIT_OOM);
    }

    return p;
}

void *urealloc(void *ptr, size_t size)
{
    /* Standard guarantees that:
     *     realloc(ptr, 0)  - behaves like the free
     *     realloc(0, size) - behaves like malloc
     * Re-allocating NULL pointer to zero bytes summons implementation
     * defined behavior, better to bail out with assert.
     */
    UASSERT_INPUT(ptr || size);

    void *p = realloc(ptr, size);

    if (!p)
    {
        if (_oom_handler(_oom_data))
        {
            p = realloc(ptr, size);
        }
    }

    if (!p)
    {
        fprintf(stderr, "out of memory error\n");
        utrace_print();
        exit(UGENERIC_EXIT_OOM);
    }

    return p;
}

void ufree(void *ptr)
{
    free(ptr);
}

void *umemdup(const void *src, size_t n)
{
    UASSERT_INPUT(src);
    UASSERT_INPUT(n);
    return memcpy(umalloc(n), src, n);
}

static void _reserve_capacity(ubuffer_t *buf, size_t new_capacity)
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

void ubuffer_append_data(ubuffer_t *buf, const void *data, size_t size)
{
    UASSERT_INPUT(buf);
    UASSERT_INPUT(data);

    _reserve_capacity(buf, buf->data_size + size);
    memcpy((char *)buf->data + buf->data_size, data, size);
    buf->data_size += size;
}

void ubuffer_append_memchunk(ubuffer_t *buf, const umemchunk_t *chunk)
{
    UASSERT_INPUT(buf);
    UASSERT_INPUT(chunk);

    _reserve_capacity(buf, buf->data_size + chunk->size);
    memcpy((char *)buf->data + buf->data_size, chunk->data, chunk->size);
    buf->data_size += chunk->size;
}

void ubuffer_append_byte(ubuffer_t *buf, char byte)
{
    UASSERT_INPUT(buf);

    _reserve_capacity(buf, buf->data_size + 1);
    ((char *)buf->data)[buf->data_size] = byte;
    buf->data_size += 1;
}

void ubuffer_append_string(ubuffer_t *buf, const char *str)
{
    UASSERT_INPUT(buf);
    UASSERT_INPUT(str);

    size_t slen = strlen(str);
    _reserve_capacity(buf, buf->data_size + slen);
    memcpy((char *)buf->data + buf->data_size, str, slen);
    buf->data_size += slen;
}

void ubuffer_null_terminate(ubuffer_t *buf)
{
    UASSERT_INPUT(buf);
    if (buf->data_size && (((char *)buf->data)[buf->data_size - 1] != '\0'))
    {
        ubuffer_append_byte(buf, '\0');
    }
}

void ubuffer_reset(ubuffer_t *buf)
{
    UASSERT_INPUT(buf);
    buf->data_size = 0;
}

int umemchunk_fprint(umemchunk_t m, FILE *out)
{
    UASSERT_INPUT(out);

    char *str = umemchunk_as_str(m);
    int ret = fprintf(out, "%s\n", str);
    ufree(str);

    return ret;
}

int umemchunk_print(umemchunk_t m)
{
    return umemchunk_fprint(m, stdout);
}

void umemchunk_serialize(umemchunk_t m, ubuffer_t *buf)
{
    UASSERT_INPUT(buf);
    UASSERT_INPUT(m.data);
    UASSERT_INPUT(m.size < SIZE_MAX / 2);

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

char *umemchunk_as_str(umemchunk_t m)
{
    ubuffer_t buf = {0};
    umemchunk_serialize(m, &buf);
    ubuffer_null_terminate(&buf);

    return buf.data;
}
