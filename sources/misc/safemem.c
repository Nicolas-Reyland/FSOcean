//
// Created by nicolas on 22. 7. 10.
//

#include <stdlib.h>
#include "misc/safemem.h"

void * reg_malloc(size_t size)
{
    return malloc(size);
}

void * reg_calloc(size_t nmemb, size_t size)
{
    return calloc(nmemb, size);
}

void reg_free(void * ptr)
{
    free(ptr);
}
