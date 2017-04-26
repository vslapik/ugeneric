#ifndef BUFFER_H__
#define BUFFER_H__

#include <stdio.h> // FILE*
#include <stddef.h> // size_t

#define BUFFER_INITIAL_CAPACITY 16
#define BUFFER_SCALE_FACTOR 2

typedef struct {
    void *data;
    size_t data_size;
    size_t capacity;
} buffer_t;

typedef struct {
    size_t size;
    void *data;
} memchunk_t;

void buffer_append_data(buffer_t *buf, const void *data, size_t size);
void buffer_append_memchunk(buffer_t *buf, const memchunk_t *data);
void buffer_append_buffer(buffer_t *buf, const buffer_t *data);
void buffer_append_byte(buffer_t *buf, char byte);
void buffer_append_string(buffer_t *buf, const char *str);
void buffer_null_terminate(buffer_t *buf);
void buffer_reset(buffer_t *buf);

char *memchunk_as_str(memchunk_t m);
void memchunk_serialize(memchunk_t m, buffer_t *buf);
int memchunk_fprint(memchunk_t m, FILE *out);
int memchunk_print(memchunk_t m);

#endif
