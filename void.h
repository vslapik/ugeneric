#ifndef UVOID__H__
#define UVOID__H__

#include "generic.h"

typedef struct {
    void_cpy_t cpy;
    void_cmp_t cmp;
    void_dtr_t dtr;
    void_s8r_t s8r;
} uvoid_handlers_t;

#define DECLARE_VOID_FUNCS(_container_)                                                                                                         \
static inline void _container_##_set_void_destroyer(_container_##_t *self, void_dtr_t dtr) {_container_##_get_void_handlers(self)->dtr = dtr;}       \
static inline void _container_##_set_void_comparator(_container_##_t *self, void_cmp_t cmp) {_container_##_get_void_handlers(self)->cmp = cmp;}     \
static inline void _container_##_set_void_copier(_container_##_t *self, void_cpy_t cpy) {_container_##_get_void_handlers(self)->cpy = cpy;}         \
static inline void _container_##_set_void_serializer(_container_##_t *self, void_s8r_t serializer) {_container_##_get_void_handlers(self)->s8r = serializer;} \
static inline void_dtr_t _container_##_get_void_destroyer(_container_##_t *self) { return _container_##_get_void_handlers(self)->dtr; }       \
static inline void_cmp_t _container_##_get_void_comparator( _container_##_t *self) { return _container_##_get_void_handlers(self)->cmp; }      \
static inline void_cpy_t _container_##_get_void_copier(_container_##_t *self) { return _container_##_get_void_handlers(self)->cpy; }          \
static inline void_s8r_t _container_##_get_void_serializer(_container_##_t *self) { return _container_##_get_void_handlers(self)->s8r; }      \

#endif
