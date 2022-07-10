//
// Created by nicolas on 22. 7. 10.
//

#ifndef OCEAN_SAFEMEM_H
#define OCEAN_SAFEMEM_H

#include <stdbool.h>
#include <stddef.h>

#define START_MEMORY_POINTERS_REGISTRY_SIZE 10

extern bool REG_MEMORY_INIT;
extern void ** REGISTERED_MEMORY_POINTERS;
extern size_t NUM_REGISTERED_MEMORY_POINTERS;
extern size_t REGISTERED_MEMORY_POINTERS_VECTOR_SIZE;

void init_memory_registration(void);

void * reg_malloc(size_t size);

void * reg_calloc(size_t nmemb, size_t size);

void * reg_realloc(void * ptr, size_t size);

void reg_free(void *);

void free_all_registered_memory(void);

#endif // OCEAN_SAFEMEM_H
