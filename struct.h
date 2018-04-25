#ifndef USTRUCT_H__
#define USTRUCT_H__

#include "dict.h"
#include "vector.h"

typedef struct ustruct_data_descriptor {
    const char *name;
    ugeneric_type_e type;
    size_t offset;
    ugeneric_t dflt;
    bool is_array;
    struct ustruct_data_descriptor *field_descriptor; // for nested structures
    size_t field_size; // for nested structures
} ustruct_data_descriptor_t;

ugeneric_t ustruct_create_from_dict(const udict_t *d, size_t struct_size,
                                    const ustruct_data_descriptor_t *sdd);

void ustruct_destroy_by_descriptor(void *p,
                                   const ustruct_data_descriptor_t *sdd);

#endif
