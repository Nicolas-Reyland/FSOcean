//
// Created on 12/05/2022.
//

#ifndef OCEAN_AST_H
#define OCEAN_AST_H

#include <stddef.h>
#include "stack.h"

typedef enum {
    NONE,
} AbstractNodeType;

typedef struct ASTNode {
    AbstractNodeType type;
    Token token;
    struct ASTNode * children;
    size_t num_children;
} ASTNode;

#endif // OCEAN_AST_H
