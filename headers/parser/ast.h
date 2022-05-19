//
// Created on 12/05/2022.
//

#ifndef OCEAN_AST_H
#define OCEAN_AST_H

#include <stddef.h>
#include "stack.h"
#include "cst.h"

typedef enum {
    AST_NONE,
    // unix shell command language
    AST_COMMAND,
    AST_FOR_LOOP,
    AST_WHILE_LOOP,
    AST_IF_STATEMENT,
} AbstractNodeType;

typedef struct ASTNode {
    AbstractNodeType type;
    char * str;
    size_t str_len;
    struct ASTNode * children;
    size_t num_children;
} ASTNode;

ASTNode abstract_cst(CSTNode node);

#endif // OCEAN_AST_H
