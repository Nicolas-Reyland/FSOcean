//
// Created by nicolas on 22. 7. 10.
//

// regex for finding usages of non-(reg_*) memory calls : [^_]([mc]|re)alloc

#include <stdlib.h>
#include <assert.h>
#include "misc/safemem.h"

#ifdef OCEAN_DEBUG_MEMORY
#include <stdio.h>
#include "misc/output/output.h"
#endif

bool REG_MEMORY_INIT = false;
void ** REGISTERED_MEMORY_POINTERS = NULL;
size_t NUM_REGISTERED_MEMORY_POINTERS = 0;
size_t REGISTERED_MEMORY_POINTERS_VECTOR_SIZE = 0;

void init_memory_registration(void)
{
    assert(!REG_MEMORY_INIT);
    assert(REGISTERED_MEMORY_POINTERS == NULL);
    assert(NUM_REGISTERED_MEMORY_POINTERS == 0);
    assert(REGISTERED_MEMORY_POINTERS_VECTOR_SIZE == 0);
    REGISTERED_MEMORY_POINTERS = calloc(START_MEMORY_POINTERS_REGISTRY_SIZE, sizeof(void *));
    REGISTERED_MEMORY_POINTERS_VECTOR_SIZE = START_MEMORY_POINTERS_REGISTRY_SIZE;
    REG_MEMORY_INIT = true;
}

static void register_new_ptr(void * ptr);
static void replace_registered_ptr(void * old_ptr, void * new_ptr);
static void remove_registered_ptr(void * ptr);

void * reg_malloc(size_t size)
{
    assert(REG_MEMORY_INIT);
    void * ptr = malloc(size);
    register_new_ptr(ptr);
    return ptr;
}

void * reg_calloc(size_t nmemb, size_t size)
{
    assert(REG_MEMORY_INIT);
    void * ptr = calloc(nmemb, size);
    register_new_ptr(ptr);
    return ptr;
}

void * reg_realloc(void * ptr, size_t size)
{
    assert(REG_MEMORY_INIT);
    void * new_ptr = realloc(ptr, size);
    replace_registered_ptr(ptr, new_ptr);
    return new_ptr;
}

void reg_free(void * ptr)
{
    assert(REG_MEMORY_INIT);
    remove_registered_ptr(ptr);
    free(ptr);
}

static void register_new_ptr(void * ptr)
{
    if (NUM_REGISTERED_MEMORY_POINTERS == REGISTERED_MEMORY_POINTERS_VECTOR_SIZE) {
        REGISTERED_MEMORY_POINTERS_VECTOR_SIZE *= 2;
        REGISTERED_MEMORY_POINTERS = realloc(REGISTERED_MEMORY_POINTERS, REGISTERED_MEMORY_POINTERS_VECTOR_SIZE * sizeof(void *));
    }
    REGISTERED_MEMORY_POINTERS[NUM_REGISTERED_MEMORY_POINTERS++] = ptr;
}

static void replace_registered_ptr(void * old_ptr, void * new_ptr)
{
    for (size_t i = 0; i < NUM_REGISTERED_MEMORY_POINTERS; i++) {
        if (REGISTERED_MEMORY_POINTERS[i] == old_ptr) {
            REGISTERED_MEMORY_POINTERS[i] = new_ptr;
            return;
        }
    }
    // old pointer is not in the registered pointers !
#ifdef OCEAN_DEBUG_MEMORY
    print_error(OCERR_STDOUT, "old ptr was not registered in realloc\n");
#endif
}

static void remove_registered_ptr(void * ptr)
{
    for (size_t i = 0; i < NUM_REGISTERED_MEMORY_POINTERS; i++) {
        if (REGISTERED_MEMORY_POINTERS[i] == ptr) {
            // removing the last pointer in the list
            if (i == NUM_REGISTERED_MEMORY_POINTERS - 1) {
                REGISTERED_MEMORY_POINTERS[i] = NULL;
                NUM_REGISTERED_MEMORY_POINTERS--;
                return;
            }
            // remove a pointer in the middle of the list
            // overwrite current address with last pointer and decrement number of pointers
            NUM_REGISTERED_MEMORY_POINTERS--;
            REGISTERED_MEMORY_POINTERS[i] = REGISTERED_MEMORY_POINTERS[NUM_REGISTERED_MEMORY_POINTERS];
            REGISTERED_MEMORY_POINTERS[NUM_REGISTERED_MEMORY_POINTERS] = NULL;
            return;
        }
    }
    // pointer not in the list !
#ifdef OCEAN_DEBUG_MEMORY
    print_error(OCERR_STDOUT, "trying to free a ptr that is not in the list\n");
#endif
}


void free_all_registered_memory(void)
{
    assert(REG_MEMORY_INIT);
    assert(REGISTERED_MEMORY_POINTERS != NULL);
    for (size_t i = 0; i < NUM_REGISTERED_MEMORY_POINTERS; i++) {
#ifdef OCEAN_DEBUG_MEMORY
        printf("ptr %p was not freed\n", REGISTERED_MEMORY_POINTERS[i]);
#endif
        free(REGISTERED_MEMORY_POINTERS[i]);
    }
    free(REGISTERED_MEMORY_POINTERS);
    REG_MEMORY_INIT = false;
    REGISTERED_MEMORY_POINTERS = NULL;
    NUM_REGISTERED_MEMORY_POINTERS = 0;
    REGISTERED_MEMORY_POINTERS_VECTOR_SIZE = START_MEMORY_POINTERS_REGISTRY_SIZE;
}
