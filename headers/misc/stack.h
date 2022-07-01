//
// Created on 12/05/2022.
//

#ifndef OCEAN_STACK_H
#define OCEAN_STACK_H

#include <stddef.h>

struct StackNode {
    int value;
    struct StackNode * next;
};

struct Stack {
    size_t size;
    struct StackNode * head;
    void (*push)(struct Stack *, int);
    int (*pop)(struct Stack *);
    int (*peek)(struct Stack *);
};

struct Stack create_stack(void);

void free_stack(struct Stack stack);

#endif // OCEAN_STACK_H
