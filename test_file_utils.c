#include "file_utils.h"
#include "mem.h"

int main(void)
{
    generic_t g;

    g = file_open("", "r");
    ASSERT(G_IS_ERROR(g));
 //   generic_error_print(g);
    generic_error_destroy(g);

    g = file_open("/root", "r");
    ASSERT(G_IS_ERROR(g));
//    generic_error_print(g);
    generic_error_destroy(g);
/*
    g = file_open("/home/vslapik/sss", "w");
    ASSERT_NO_ERROR(g);
    FILE *fd = G_AS_PTR(g);

    size_t s = 4333333311LL;
    void *b = umalloc(s);
    if (s != fwrite(b, s, 1, fd))
    {
        g = G_ERROR_IO;
        ASSERT_NO_ERROR(g);
    }
    puts("OK");
    */

    return 0;
}
