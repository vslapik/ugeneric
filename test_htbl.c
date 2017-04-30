#include "generic.h"
#include "mem.h"
#include "htbl.h"
#include "string_utils.h"

int main(void)
{
    uhtbl_t *h = uhtbl_create();
    uhtbl_put(h, G_STR(ustring_dup("one")), G_STR(ustring_dup("one")));
    ugeneric_t g = uhtbl_pop(h, G_STR("one"), G_NULL);
    UASSERT(ugeneric_get_type(g) == G_STR_T);
    ufree(G_AS_STR(g));
    uhtbl_destroy(h);
}
