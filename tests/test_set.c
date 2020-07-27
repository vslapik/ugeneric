#include "set.h"

#include "file_utils.h"

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    uset_t *s = uset_create();

    uset_put(s, G_INT(1));
    uset_put(s, G_INT(2));
    uset_put(s, G_INT(3));
    uset_put(s, G_INT(1));
    uset_put(s, G_INT(1));

//    uset_print(s);
    uset_destroy(s);

/*

    ugeneric_t g = ufile_read_to_string("mars.txt");
    UASSERT(G_IS_STR(g));
    char *str = G_AS_STR(g);
    str = ustring_replace_char(str, '\n', ' ');
//    puts(str);

    uvector_t *v = ustring_split(str, " ");
    uvector_sort(v);
    //uvector_print(v);

    uset_t *s = uset_create();
    uset_update_from_vector(s, v, true);
    uset_print(s);

    ufree(str);

    ugeneric_destroy(g, NULL);
    uvector_destroy(v);
    uset_destroy(s);
    */

    return EXIT_SUCCESS;
}
