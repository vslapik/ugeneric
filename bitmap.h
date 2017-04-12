#ifndef BITMAP_H__
#define BITMAP_H__

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct bitmap_opaq bitmap_t;

bitmap_t *bitmap_create(size_t size);
void bitmap_destroy(bitmap_t *b);
size_t bitmap_get_size(const bitmap_t *b);
void bitmap_set_bit(bitmap_t *b, size_t i);
bool bitmap_get_bit(const bitmap_t *b, size_t i);
void bitmap_clear_bit(bitmap_t *b, size_t i);
void bitmap_set_range(bitmap_t *b, size_t l, size_t r);
void bitmap_swap_bits(bitmap_t *b, size_t i1, size_t i2);
void bitmap_swap_ranges(bitmap_t *b, size_t l1, size_t r1,
                        size_t l2, size_t r2);
void bitmap_reverse_range(bitmap_t *b, size_t l, size_t r);
void bitmap_reverse(bitmap_t *b);
void bitmap_flip_all(bitmap_t *b);
void bitmap_flip_range(bitmap_t *b, size_t l, size_t r);

char *bitmap_range_as_str(const bitmap_t *b, size_t l, size_t r);
char *bitmap_as_str(const bitmap_t *b);
int bitmap_fprint_range(const bitmap_t *b, size_t l, size_t r, FILE *f);
int bitmap_fprint(const bitmap_t *b, FILE *f);

#endif
