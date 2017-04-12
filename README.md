The library is a pile of code around concept of generic data item represented by 'generic_t' type. It is not well structured, not well tested, not production ready, contains a lot of bugs, lacks UT coverage, lacks documentation and has no clear development roadmap. However I'd love to fix all above sooner or later and keep this repository as a set of reusable pieces of code for my other projects. The library routines are not supposed to be super fast but rather convenient to use. Also there is no clear library boundaries, it is just rather scattered files which can be separately copied in another project to provide some pieces of functionality. Below some random notes instead of documentation which is apparently missing so far.

The generic_t is a tagged union capable to represent most of the widely used C data types (for sure at some added cost). There are primitive types which can be wrapped by generic_t (like signed/unsigned integers, strings, booleans, pointers) and two composite types:
- dynamically resizing array (or vector or list or ...)
- dictionary (or associative array or map or whatever name you like).

Also pointers to some arbitrary data can be stored in containers. You can copy, compare and destroy this arbitrary data if you register appropriate functions (void_cpy_t, void_cmp_t, void_dtr_t).

In addition vector and dictionary there are also other widely used data structures supported like heap, stack, queue, linked list, disjoint set union, bitmap and some other which are mostly not ready
All containers works with generic_t, i.e. list and vector elements, dict keys and values, queue elements, stack elements - all of them are of generic_t type.
Variables of generic_t type are passed to / returned from functions by value (there are only a few exceptions). For all utilities which just manipulate the data structures there is no return code which needs to be tested after each call. Justification is following. There are just few things which may go wrong during execution of some API call:
1. User of the API is an idiot and passed junk to a function (null pointer when valid pointer to data is expected, attempt to pop an element from an empty stack, getting an element from a vector using index which is greater than vector size, using iterator when iteration is completed and so on). The strategy is to do nothing to prevent the user from doing it. It might be a bit unexpected if you came from languages which control each and every your action. But this is a good C tradition, not to prevent such cases and trust the programmer. Try to pass NULL pointer, say to strlen(). It won't cry out, it will just merely crash and investigation of why it crashes is your responsibility. For convenience all API input is covered with asserts which can be optionally enabled. If user of API is able to pass junk it is questionably whether he or she would be able to analyze the return code...
2. There is a bug and under some conditions internal data structure is corrupted. It means the code is broken and shouldn't be used until it gets fixed. Return codes wouldn't help here either. There some internal asserts for such conditions which are enabled by default in order to shut down execution as soon as possible when such situation is detected. Normally it shouldn't occur but I warned you, the stuff is not that well tested yet.
3. Internal attempt to allocate memory failed. This is probably the most difficult situation to recover from and here return code would probably make sense by passing responsibility of handling such a situation to a caller. However in most situations callers don't care about it. If malloc or friends returned 0 most likely this is either ignored or logged then application is aborted. The approach in this library is a bit different. No return code is used anyway but you can try foresee such a situation by registering a callback which will be called when memory allocation is failed. The callback is expected to free some unused memory then initial allocation will be re-executed. If allocation still fails after the second attempt there is no way to proceed, so exit() just called and log message is produced to stderr. Calling exit() allows to execute some additional actions you may have registered by atexit() or friends like closing file descriptors, saving your precious data, informing your user and so on (no guarantees it will be performed successfully as we ran out of memory, but that's a different story). To complicate the story further, some environments never report memory exhaustion by returning null pointer from memory allocation routines (google for Linux overcommit) so there is little chance you will be able to correctly handle memory exhaustion anyway.

Random notes:
- library code is NOT thread safe, locks are caller responsibility
- xxx_destroy(NULL) safely does nothing similar to free(NULL)
- containers are owners of elements memory by default, i.e. all elements memory is freed when container is destroyed, you don't need to keep pointers for everyting you put to container
- you can serialize containers to JSON compatible format or construct containers from JSON, not all JSON features are supported (like unicode strings, some odd float points formats, etc)
- xxx_remove_xxx - remove element from container and destroy it without returning to caller
- xxx_pop_xxx - remove element from container and return to caller (caller is responsible for destroying element)
- xxx_get/peek_xxx - return element to caller without removing it from container (shallow copy)
- xxx_push_xxx - insert element to container
