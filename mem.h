#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#ifndef UMEM_H__
#define UMEM_H__

typedef bool (*oom_handler_t)(void *data);
void libugeneric_set_oom_handler(oom_handler_t handler, void *ctx);

void *umalloc(size_t size);
void *ucalloc(size_t nmemb, size_t size);
void *urealloc(void *uptr, size_t nmemb);
void ufree(void *ptr);
static inline void *uzalloc(size_t size) { return ucalloc(size, 1); }

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
