#include <stdlib.h>
#include <stdio.h>
#include "deque.h"

struct uvector_opaq {
    ugeneric_t *data;
    size_t size;
    bool is_data_owner;
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

