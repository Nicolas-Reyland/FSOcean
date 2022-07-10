//
// Created by nicolas on 22. 7. 10.
//

#ifndef OCEAN_SAFEMEM_H
#define OCEAN_SAFEMEM_H

#include <stddef.h>

void * reg_malloc(size_t size);

void * reg_calloc(size_t nmemb, size_t size);

void reg_free(void *);

#endif //OCEAN_SAFEMEM_H
