//
// Created on 12/05/2022.
//

#ifndef OCEAN_AST_H
#define OCEAN_AST_H

#include <stddef.h>
#include "token_stack.h"

typedef enum {
    NONE,
} NodeType;

typedef struct ASTNode {
    NodeType type;
    TokenStackNode node;
    struct ASTNode * children;
    size_t num_children;
} ASTNode;

#endif // OCEAN_AST_H
