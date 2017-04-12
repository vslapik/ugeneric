#ifndef STRING_UTILS_H__
#define STRING_UTILS_H__

#include "vector.h"

vector_t *string_split(const char *str, const char *sep);
char *string_dup(const char *str);
char *string_ndup(const char *str, size_t n);
char *string_fmt(const char *fmt, ...);
bool string_starts_with(const char *str, const char *prefix);

#endif
