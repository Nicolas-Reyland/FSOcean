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
    //
    AST_COMMAND_LIST,

    AST_COMMAND_SCOPE,
    AST_COMMAND_CLASSIC,

    AST_REDIRECTS,
    AST_REDIRECT_IN,
    AST_REDIRECT_OUT,

    AST_NAMES,
    AST_PREFIXES,
    //
    AST_IF_STATEMENT,

    AST_FOR_LOOP,
    AST_WHILE_LOOP,
    AST_UNTIL_LOOP,

    AST_CASE_STATEMENT,
    AST_CASE_EXPR,
    AST_CASE_MATCHES,

    AST_CONDITIONAL_BRANCH,
    //

    AST_VALUE,
    AST_EVAL_VALUE,

    //
    AST_EMPTY,
} AbstractNodeType;

typedef struct ASTNode {
    AbstractNodeType type;
    char * str;
    size_t str_len;
    struct ASTNode * children;
    size_t num_children;
} ASTNode;

extern const char * const ABSTRACT_NODE_TYPE_STRING[];

ASTNode abstract_cst(CSTNode cst_node);

#endif // OCEAN_AST_H
