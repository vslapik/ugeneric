#ifndef USTRING_UTILS_H__
#define USTRING_UTILS_H__

#include "vector.h"
#include "mem.h"

uvector_t *ustring_split(const char *str, const char *sep);
char *ustring_replace_char(const char *str, char from, char to);
char *ustring_dup(const char *str);
char *ustring_ndup(const char *str, size_t n);
char *ustring_fmt(const char *fmt, ...);
char *ustring_fmt_sized(const char *fmt, size_t *output_size, ...);
bool ustring_starts_with(const char *str, const char *prefix);

#endif
