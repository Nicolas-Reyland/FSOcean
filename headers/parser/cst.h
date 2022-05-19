//
// Created on 16/05/2022.
//

#ifndef OCEAN_CST_H
#define OCEAN_CST_H

#include <stddef.h>
#include "stack.h"

typedef enum {
    CST_NONE,
    // unix shell command language peg rules names
    CST_SHELL_INSTRUCTION,

    CST_COMMAND,
    CST_COMMAND_UNIT,

    CST_COMMAND_PREFIX,
    CST_SCOPE_COMMAND,

    CST_NAME,
    CST_NAMES,

    CST_REDIRECT,
    CST_REDIRECT_IN,
    CST_REDIRECT_OUT,

    CST_CMD_SEP,
    CST_PIPE,
    CST_AMP,
    CST_NEW_CMD,

    CST_LITERAL,
    CST_DOUBLEQ,
    CST_SINGLEQ,

    CST_IF_STATEMENT,
    CST_IF_CONDITION_ACTION,
    CST_IF_ALTERNATIVE,
    CST_FOR_LOOP,
    CST_LOOP_BODY,
    CST_WHILE_LOOP,
    CST_UNTIL_LOOP,
    CST_CASE_STATEMENT,
    CST_CASE_EXPR,


    // auto-type special parsers
    CST_SEQUENCE,
    CST_SEQUENCE_UNIT,
    CST_REPETITION,
    CST_CHOICE,
    CST_OPTIONAL,

    CST_SEPARATED,
    CST_SEPARATED_REPETITION,

    // Generator
    CST_GENERATOR,

    // Macros
    CST_STRING_PARSER,
    CST_STRING_STATE_PARSER,
    CST_STATE_PARSER,
} ConcreteNodeType;

extern const char * const CONCRETE_NODE_TYPE_STRING[];

typedef struct CSTNode {
    ConcreteNodeType type;
    Token * token;
    struct CSTNode ** children;
    size_t num_children;
} CSTNode;

void free_cst_node(CSTNode * node);
void free_cst_node_children(CSTNode node);
void prune_cst(CSTNode * node);

#endif // OCEAN_CST_H
