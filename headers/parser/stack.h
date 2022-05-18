//
// Created on 12/05/2022.
//

#ifndef OCEAN_STACK_H
#define OCEAN_STACK_H

#include <stddef.h>
#include "lexer/states.h"
#include "lexer/token.h"

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

#endif // OCEAN_STACK_H
