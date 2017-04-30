#ifndef UBUFFER_H__
#define UBUFFER_H__

#include <stdio.h> // FILE*
#include <stddef.h> // size_t

#define BUFFER_INITIAL_CAPACITY 16
#define BUFFER_SCALE_FACTOR 2

typedef struct {
    void *data;
    size_t data_size;
    size_t capacity;
} ubuffer_t;

typedef struct {
    size_t size;
    void *data;
} umemchunk_t;

void ubuffer_append_data(ubuffer_t *buf, const void *data, size_t size);
void ubuffer_append_memchunk(ubuffer_t *buf, const umemchunk_t *data);
void ubuffer_append_buffer(ubuffer_t *buf, const ubuffer_t *data);
void ubuffer_append_byte(ubuffer_t *buf, char byte);
void ubuffer_append_string(ubuffer_t *buf, const char *str);
void ubuffer_null_terminate(ubuffer_t *buf);
void ubuffer_reset(ubuffer_t *buf);

char *umemchunk_as_str(umemchunk_t m);
void umemchunk_serialize(umemchunk_t m, ubuffer_t *buf);
int umemchunk_fprint(umemchunk_t m, FILE *out);
int umemchunk_print(umemchunk_t m);

#endif
