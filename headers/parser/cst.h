//
// Created on 16/05/2022.
//

#ifndef OCEAN_CST_H
#define OCEAN_CST_H

#include <stddef.h>
#include "stack.h"

typedef enum {
    NONE,
    COMMAND,
    COMMAND_TAIL,
    NAME,
    NAMES,
    CMD_SEP,
    PIPE,
    AMP,
    NEW_CMD,
    LITERAL,
    DOUBLEQ,
    SINGLEQ,
} ConcreteNodeType;

extern const char * const CONCRETE_NODE_TYPE_STRING[];

typedef struct CSTNode {
    ConcreteNodeType type;
    Token token;
    struct CSTNode ** children;
    size_t num_children;
} CSTNode;

/*
 * Parser combinators :
 */


#endif // OCEAN_CST_H
