#include <stdlib.h>
#include <stdbool.h>

#ifndef MEM_H__
#define MEM_H__

typedef bool (*oom_handler_t)(void *data);
void libgnrc_set_oom_handler(oom_handler_t handler, void *data);

void *umalloc(size_t size);
void *ucalloc(size_t nmemb, size_t size);
void *urealloc(void *uptr, size_t nmemb);
void ufree(void *ptr);

#endif
