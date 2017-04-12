#include "generic.h"
#include "mem.h"
#include "htbl.h"
#include "string_utils.h"

int main(void)
{
    htbl_t *h = htbl_create();
    htbl_put(h, G_STR(string_dup("one")), G_STR(string_dup("one")));
    generic_t g = htbl_pop(h, G_STR("one"), G_NULL);
    ASSERT(generic_get_type(g) == G_STR_T);
    ufree(G_AS_STR(g));
    htbl_destroy(h);
}
