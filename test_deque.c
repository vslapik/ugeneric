#include <stdlib.h>
#include <stdio.h>
#include "deque.h"

struct vector_opaq {
    generic_t *data;
    size_t size;
    bool is_data_owner;
    void_cpy_t cpy;
    void_cmp_t cmp;
    void_dtr_t dtr;
};

void test_deque_api(void)
{
//    deque_t *d = deque_create();
}

int main(void)
{
    test_deque_api();
    return EXIT_SUCCESS;
}

