//
// Created on 12/05/2022.
//

#ifndef OCEAN_TOKEN_STACK_H
#define OCEAN_TOKEN_STACK_H

#include <stddef.h>
#include "lexer/states.h"

typedef struct TokenStackNode {
    char * str;
    size_t str_len;
    STATE state;
    struct TokenStackNode * next;
} TokenStackNode;

typedef struct {
    size_t size;
    TokenStackNode * head;
} TokenStack;

#endif // OCEAN_TOKEN_STACK_H
